#include "Shell.h"

Shell::Shell(Simdisk& simdisk, NamedPipe& pipe, uint8_t userID)
	:simdisk(simdisk), namedPipe(pipe), userID(userID), usedFlag(1), currentDir(&simdisk.getFile(simdisk.getSuperBlock().getRootInode())), oss(string()), currentDirname("/") {
	currenInodeSet.insert(simdisk.getSuperBlock().getRootInode());
}

bool Shell::getStatus() {
	return usedFlag;
}

string Shell::read() {
	return namedPipe.read();
}

void Shell::commandLine(istringstream iss, CRITICAL_SECTION& semaphore) {
	bool largeBuffer = 0;
	currentDir.file2Map();
	string userInput;
	iss >> userInput;
	if (userInput == "EXIT") {
		closeShell(semaphore);
	}
	if (userInput == "info") {
		if (iss >> userInput) {
			userInput == "-h" ? showInfoHelp() : throw string("invalid command");
		}
		else {
			showInfo();
		}
	}
	else if (userInput == "cd") {
		if (iss >> userInput) {
			EnterCriticalSection(&semaphore);
			cd(userInput, semaphore);
		}
		else {
			throw string("Please input target path!");
		}
	}
	else if (userInput == "dir") {
		Directory targetDir = currentDir;
		uint32_t count = 0;
		EnterCriticalSection(&semaphore);
		if (iss >> userInput) {
			if (userInput != "-s") {
				targetDir = simdisk.openDir(userInput, currentDir);
				if (iss >> userInput) {
					if (userInput == "-s") {
						count++;
					}
					else {
						LeaveCriticalSection(&semaphore);
						throw string("invalid command");
					}
				}
			}
			else {
				count++;
			}
		}
		if (!targetDir.testRead(userID)) {
			LeaveCriticalSection(&semaphore);
			throw string("you are not allowed to read this dir");
		}
		oss << "\t<type>name inodeID authority UID filesize\n";
		if (iss >> userInput) {
			if (userInput == "-sorted") {
				showDirSorted(targetDir, count);
			}
			else {
				LeaveCriticalSection(&semaphore);
				throw string("invalid input");
			}
		}
		else {
			showDir(targetDir, count);
		}
		LeaveCriticalSection(&semaphore);
	}
	else if (userInput == "md") {
		if (!(iss >> userInput)) {
			throw string("incomplete command");
		}
		else if (userInput.find('/') != string::npos) {
			throw string("unacceptable dir name");
		}
		string dirname = userInput;
		Directory targetDir = currentDir;
		EnterCriticalSection(&semaphore);
		if (iss >> userInput) {
			targetDir = simdisk.openDir(userInput, currentDir);
		}
		if (!targetDir.testWrite(userID)) {
			LeaveCriticalSection(&semaphore);
			throw string("you are not allowed to write this dir");
		}
		try {
			simdisk.createDir(userID, targetDir, dirname);
		}
		catch (const string errorMessage) {
			LeaveCriticalSection(&semaphore);
			throw errorMessage;
		}
		LeaveCriticalSection(&semaphore);
	}
	else if (userInput == "rd") {
		if (!(iss >> userInput)) {
			throw string("incomplete command");
		}
		EnterCriticalSection(&semaphore);
		Directory targetDir = currentDir;
		try {
			targetDir = simdisk.openDir(userInput, currentDir);
		}
		catch (const string message) {
			throw message;
		}
		if (!targetDir.testWrite(userID)) {
			LeaveCriticalSection(&semaphore);
			throw string("you are not allowed to write this dir");
		}
		uint16_t targetDirInodeID = targetDir["."].getInodeID();
		if (!targetDirInodeID) {
			LeaveCriticalSection(&semaphore);
			throw string("can not delete the root dir");
		}
		if (currenInodeSet.count(targetDirInodeID)) {
			LeaveCriticalSection(&semaphore);
			throw string("can not delete the dir others using");
		}
		if (targetDir.getMapSize() > 2) {
			oss << "target dir is not empty,proceed(Y/n)?\n";
			ossSend();
			istringstream confirmMessage = getBuffer(namedPipe.read());
			string confirm;
			confirmMessage >> confirm;
			if (confirm == "N" || confirm == "n") {
				LeaveCriticalSection(&semaphore);
				ossSend();
				return;
			}
			else if (confirm == "Y" || confirm == "y") {
				deleteDir(userInput, targetDir, targetDirInodeID, semaphore);
			}
			else {
				LeaveCriticalSection(&semaphore);
				throw string("invalid command\n");
			}
		}
		else {
			deleteDir(userInput, targetDir, targetDirInodeID, semaphore);
		}
	}
	else if (userInput == "newfile") {
		if (!(iss >> userInput)) {
			throw string("incomplete command");
		}
		string filename = userInput;
		EnterCriticalSection(&semaphore);
		//仅支持在当前文件夹新建文件
		if (!currentDir.testWrite(userID)) {
			LeaveCriticalSection(&semaphore);
			throw string("you are not allowed to write this dir");
		}
		if (!(iss >> userInput)) {
			LeaveCriticalSection(&semaphore);
			throw string("incomplete command");
		}
		if (userInput == "-quit") {
			LeaveCriticalSection(&semaphore);
			ossSend();
			return;
		}
		else if (userInput == "-save") {
			newfile(currentDir, filename, semaphore);
		}
		else {
			if (userInput.size() > MAX_FILESIZE) {
				LeaveCriticalSection(&semaphore);
				throw string("newfile failed because of oversize");
			}
			else if (userInput.size() > simdisk.getAvlSpace()) {
				LeaveCriticalSection(&semaphore);
				throw string("newfile failed because of no available space");
			}
			string content = userInput;
			if (!(iss >> userInput)) {
				LeaveCriticalSection(&semaphore);
				throw string("incomplete command");
			}
			if (userInput == "-quit") {
				LeaveCriticalSection(&semaphore);
				ossSend();
				return;
			}
			else if (userInput == "-save") {
				newfile(currentDir, filename, semaphore, content);
			}
			else {
				LeaveCriticalSection(&semaphore);
				throw string("invalid command");
			}
		}
	}
	else if (userInput == "cat") {
		if (!(iss >> userInput)) {
			throw string("incomplete command");
		}
		EnterCriticalSection(&semaphore);
		uint16_t targetFileInodeID = simdisk.openFile(userInput, currentDir);
		File& file = simdisk.getFile(targetFileInodeID);
		if (!file.testRead(userID)) {
			LeaveCriticalSection(&semaphore);
			throw string("you are not allowed to read this file");
		}

		oss << string(file.getContent(), file.getInode().getFileSize()) << endl;
		simdisk.closeFile(targetFileInodeID);
		LeaveCriticalSection(&semaphore);
	}
	else if (userInput == "copy") {
		string filePath, targetPath;
		if (!(iss >> filePath >> targetPath)) {
			throw string("incomplete command");
		}
		if (!filePath.find("<host>") && !targetPath.find("<host>")) {
			throw string("can not copy from host to host");
		}
		else if (!filePath.find("<host>")) {
			EnterCriticalSection(&semaphore);
			host2simdisk(filePath, targetPath, semaphore);
		}
		else if (!targetPath.find("<host>")) {
			EnterCriticalSection(&semaphore);
			simdisk2host(filePath, targetPath, semaphore);
		}
		else {
			EnterCriticalSection(&semaphore);
			simdisk2simdisk(filePath, targetPath, semaphore);
		}
	}
	else if (userInput == "del") {
		if (!(iss >> userInput)) {
			throw string("incomplete command");
		}
		EnterCriticalSection(&semaphore);
		del(userInput, semaphore);
	}
	else if (userInput == "check") {
		check(semaphore);
	}
	else if (userInput == "save") {
		simdisk.save();
	}
	else if (userInput == "EXIT") {
		closeShell(semaphore);
	}
	else {
		throw string("invalid command");
	}
	ossSend();
}

void Shell::showInfoHelp() {
	oss << "\tcommand help: command <must_params> [options]\n";
	oss << "\tall commands:\n";
	oss << "\tinfo [-h]\n";
	oss << "\tcd <target_path>\n";
	oss << "\tdir [target_path] [-s] [-sorted]\n";
	oss << "\tmd <dir_name> [target_path]\n";
	oss << "\trd <target_path>\n";
	oss << "\tnewfile <filename> [filecontent] <-save|-quit>\n";
	oss << "\tcat <file_path>\n";
	oss << "\tcopy <file_path> [target_path]\n";
	oss << "\tdel <file_path>\n";
	oss << "\tcheck\n";
	oss << "\tclear\n";
	oss << "\tsave\n";
	oss << "\tEXIT\n";
}

void Shell::showInfo() {
	SuperBlock superBlock = simdisk.getSuperBlock();
	oss << "\tblock size: " << BLOCKSIZE / 1024 << "KB\n";
	oss << "\tdisk total space: " << DISKSIZE / 1024 << "KB\n";
	oss << "\tdisk used by system(SuperBlock/Inode/Bitmap): " << superBlock.getFirstBlockPos() / BLOCKSIZE << "KB\n";
	oss << "\tdisk avaliable space: " << superBlock.getFreeBlockCnt() << "KB\n";
	oss << "\tinode amount: " << INODE_CNT;
	oss << "\n\tinode avaliable: " << superBlock.getFreeInodeCnt();
	oss << "\n\tinode used: " << INODE_CNT - superBlock.getFreeInodeCnt() << endl;
}

void Shell::cd(string userInput, CRITICAL_SECTION& semaphore) {
	Directory targetDir = simdisk.openDir(userInput, currentDir);
	if (!targetDir.testRead(userID)) {
		LeaveCriticalSection(&semaphore);
		throw string("you are not allowed to read this dir");
	}
	currenInodeSet.erase(currenInodeSet.find(currentDir["."].getInodeID()));
	currenInodeSet.insert(targetDir["."].getInodeID());
	currentDir = targetDir;
	LeaveCriticalSection(&semaphore);
}

void Shell::showDir(Directory targetDir, uint32_t count) {
	//遍历每一个映射对并输出信息
	for (auto& n2d : targetDir.getMap()) {
		DirEntry& dirEntry = n2d.second;
		string dirname = dirEntry.getFilename();
		if (dirname == "." || dirname == "..") {
			continue;
		}
		uint16_t inodeID = dirEntry.getInodeID();
		Inode& inode = simdisk.getInode(inodeID);

		string s(count ? count - 1 : count, '\t');//创建缩进表示子目录或子文件
		bool type = dirEntry.getType();
		s += type ? "\t<dir>" : "\t<file>";
		oss << s << dirname << " " << to_string(inodeID) << " " << inode.getWRPermissions()
			<< " " << to_string(inode.getOwnerID()) << " " << to_string(inode.getFileSize()) << endl;
		if (count && type) {
			simdisk.loadFile(inodeID);
			showDir(Directory(&simdisk.getFile(inodeID)), count + 1);//递归输出子文件夹信息
		}
	}
}

void Shell::showDirSorted(Directory targetDir, uint32_t count) {
	vector<pair<string, DirEntry>>v = targetDir.Map2Vector();
	//按目录项名正序排序
	sort(v.begin(), v.end(), [](const pair<string, DirEntry>p1, const pair<string, DirEntry>p2) { return p1.first < p2.first; });
	for (auto& n2d : v) {
		DirEntry& dirEntry = n2d.second;
		string dirname = dirEntry.getFilename();
		if (dirname == "." || dirname == "..") {
			continue;
		}
		uint16_t inodeID = dirEntry.getInodeID();
		Inode& inode = simdisk.getInode(inodeID);

		string s(count ? count - 1 : count, '\t');//创建缩进表示子文件夹或子文件
		bool type = dirEntry.getType();
		s += type ? "\t<dir>" : "\t<file>";
		oss << s << dirname << " " << to_string(inodeID) << " " << inode.getWRPermissions()
			<< " " << to_string(inode.getOwnerID()) << " " << to_string(inode.getFileSize()) << endl;
		if (count && type) {
			simdisk.loadFile(inodeID);
			showDirSorted(Directory(&simdisk.getFile(inodeID)), count + 1);//递归输出子文件夹信息
		}
	}
}

void Shell::deleteDir(string userInput, Directory targetDir, uint16_t targetDirInodeID, CRITICAL_SECTION& semaphore) {
	string dirname = (userInput.rfind('/') == string::npos) ? userInput : userInput.substr(userInput.rfind('/') + 1);
	//在目标目录的父目录中删除相关信息
	uint16_t parentDirInodeID = targetDir[".."].getInodeID();
	Directory parentDir(&simdisk.getFile(parentDirInodeID));
	parentDir.deleteFile(dirname);//在父目录中删除目标目录
	simdisk.writeFileBack(parentDirInodeID);

	simdisk.deleteSubdir(targetDir);//删除目标目录的子目录
	simdisk.deleteFile(targetDirInodeID);//删除目标目录
	LeaveCriticalSection(&semaphore);
}

void Shell::newfile(Directory targetDir, string filename, CRITICAL_SECTION& semaphore, string content) {
	uint16_t newfileInodeID = simdisk.createFile();
	simdisk.setInodeBitmap(newfileInodeID);
	File& file = simdisk.getFile(newfileInodeID);
	file.setOwner(userID);
	uint32_t contentsize = content.size();
	file.resizeContent(contentsize);
	if (contentsize > 0) {
		memcpy(file.getContent(), content.c_str(), contentsize);
	}
	simdisk.writeFileBack(newfileInodeID);
	simdisk.closeFile(newfileInodeID);
	//将新文件添加到文件夹
	targetDir.addFile(newfileInodeID, filename);
	simdisk.writeFileBack(targetDir["."].getInodeID());
	LeaveCriticalSection(&semaphore);
}

void Shell::host2simdisk(string filePath, string targetPath, CRITICAL_SECTION& semaphore) {
	fstream hostfile(filePath.substr(6), ios::in | ios::binary);
	if (!hostfile) {
		LeaveCriticalSection(&semaphore);
		throw string("host file does not exist");
	}

	//获取要读取的文件信息
	string filename = filePath.substr((filePath.rfind('/') != string::npos) ? filePath.rfind('/') + 1 : 6);
	hostfile.seekg(0, ios::end);
	uint64_t filesize = hostfile.tellg();
	if (filesize > MAX_FILESIZE) {
		LeaveCriticalSection(&semaphore);
		throw string("host file is oversize");
	}
	if (filesize > simdisk.getAvlSpace()) {
		LeaveCriticalSection(&semaphore);
		throw string("no available space");
	}
	Directory targetDir = simdisk.openDir(targetPath, currentDir);
	if (!targetDir.testWrite(userID)) {
		LeaveCriticalSection(&semaphore);
		throw string("you are not allowed to write this dir");
	}

	//读取host系统的文件
	hostfile.seekg(0, ios::beg);
	char* content = new char[filesize + 1];
	content[filesize] = '\0';
	hostfile.read(content, filesize);
	hostfile.close();

	//创建新文件
	uint16_t newfileInodeID = simdisk.createFile();
	simdisk.setInodeBitmap(newfileInodeID);
	File& file = simdisk.getFile(newfileInodeID);
	file.setOwner(userID);
	file.resizeContent(filesize);

	//写入读取的内容
	memcpy(file.getContent(), content, filesize);
	delete[] content;
	simdisk.writeFileBack(newfileInodeID);

	//将新文件添加到目录
	targetDir.addFile(newfileInodeID, filename);
	simdisk.writeFileBack(targetDir["."].getInodeID());
	simdisk.closeFile(newfileInodeID);
	LeaveCriticalSection(&semaphore);
}

void Shell::simdisk2host(string filePath, string targetPath, CRITICAL_SECTION& semaphore) {
	string filename = filePath.substr(filePath.rfind('/') != string::npos ? filePath.rfind('/') + 1 : 0);
	targetPath = targetPath.substr(6);
	(!targetPath.empty()) ? targetPath.append("/") : targetPath;
	uint16_t fileInodeID = simdisk.openFile(filename, currentDir);
	simdisk.loadFile(fileInodeID);
	File& simdiskFile = simdisk.getFile(fileInodeID);
	if (!simdiskFile.testRead(userID)) {
		simdisk.closeFile(fileInodeID);
		LeaveCriticalSection(&semaphore);
		throw string("you are not allowed to read this file");
	}
	fstream hostfile(targetPath + filename, ios::out | ios::binary);
	if (!hostfile) {
		simdisk.closeFile(fileInodeID);
		LeaveCriticalSection(&semaphore);
		throw string("can not copy file to host");
	}
	hostfile.write(simdiskFile.getContent(), simdiskFile.getContentSize());
	simdisk.closeFile(fileInodeID);
	hostfile.close();
	LeaveCriticalSection(&semaphore);
}

void Shell::simdisk2simdisk(string filePath, string targetPath, CRITICAL_SECTION& semaphore) {
	uint16_t fileInodeID = simdisk.openFile(filePath, currentDir);
	File& file = simdisk.getFile(fileInodeID);
	if (!file.testRead(userID)) {
		simdisk.closeFile(fileInodeID);
		LeaveCriticalSection(&semaphore);
		throw string("you are not allowed to read this file");
	}
	Directory targetDir = simdisk.openDir(targetPath, currentDir);
	if (!targetDir.testWrite(userID)) {
		LeaveCriticalSection(&semaphore);
		throw string("you are not allowed to write this dir");
	}

	//获取要读取的文件信息
	string filename = filePath.substr(filePath.rfind('/') != string::npos ? filePath.rfind('/') + 1 : 0);
	uint32_t filesize = file.getInode().getFileSize();
	if (filesize > simdisk.getAvlSpace()) {
		LeaveCriticalSection(&semaphore);
		throw string("no available space");
	}

	//创建新文件
	uint16_t newfileInodeID = simdisk.createFile();
	simdisk.setInodeBitmap(newfileInodeID);
	File& newfile = simdisk.getFile(newfileInodeID);
	newfile.setOwner(userID);
	newfile.resizeContent(filesize);

	//将文件内容复制到新文件
	memcpy(newfile.getContent(), file.getContent(), filesize);
	simdisk.writeFileBack(newfileInodeID);

	//将新文件添加到目录
	targetDir.addFile(newfileInodeID, filename);
	simdisk.writeFileBack(targetDir["."].getInodeID());
	simdisk.closeFile(newfileInodeID);
	LeaveCriticalSection(&semaphore);
}

void Shell::del(string targetPath, CRITICAL_SECTION& semaphore) {
	uint16_t targetFileInodeID = simdisk.openFile(targetPath, currentDir);
	File& file = simdisk.getFile(targetFileInodeID);
	if (!file.testWrite(userID)) {
		LeaveCriticalSection(&semaphore);
		throw string("you are not allowed to write this file");
	}
	auto pos = targetPath.rfind('/');
	string filename = (pos == string::npos) ? targetPath : targetPath.substr(pos + 1);
	Directory targetDir = (pos == string::npos) ? currentDir : simdisk.openDir(targetPath.substr(0, pos), currentDir);
	if (targetDir[filename].getType()) {
		simdisk.closeFile(targetFileInodeID);
		LeaveCriticalSection(&semaphore);
		throw string("can not del a dir ,use command \"rd\" instead");
	}
	simdisk.deleteFile(targetFileInodeID);
	targetDir.deleteFile(filename);
	simdisk.writeFileBack(targetDir["."].getInodeID());
	LeaveCriticalSection(&semaphore);
}

void Shell::check(CRITICAL_SECTION& semaphore) {
	for (uint16_t i = 0; i < INODE_CNT; ++i) {
		if (simdisk.getInode(i).testUsed() != simdisk.testInodeBitmap(i)) {
			EnterCriticalSection(&semaphore);
			simdisk.setInodeBitmap(i, simdisk.getInode(i).testUsed());
			LeaveCriticalSection(&semaphore);
		}
	}
	EnterCriticalSection(&semaphore);
	simdisk.save();
	LeaveCriticalSection(&semaphore);
	oss << "self check and fix are done\n";
}

void Shell::closeShell(CRITICAL_SECTION& semaphore) {
	EnterCriticalSection(&semaphore);
	simdisk.save();
	LeaveCriticalSection(&semaphore);
	usedFlag = 0;
}

void Shell::ossSend() {
	namedPipe.write(oss.str());
	oss.str(string());
}

istringstream Shell::getBuffer(string message) {
	return istringstream(message);
}

void Shell::outputError(string errorMessage) {
	oss << "-e" << errorMessage << endl;
	ossSend();
}