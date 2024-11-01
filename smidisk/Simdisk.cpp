#include "Simdisk.h"

Simdisk::Simdisk(const string vDiskName) : FileOp(vDiskName) {
	uint16_t rootInodeID = 0;
	if (rootInodeID != superBlock.getRootInode()) {
		superBlock.setRootInode(0);
		superBlock.freeInodeCntDecrement();
		Inode& inode = getInode(rootInodeID);
		inode.formatting();
		inode.setUsed();
		//建立相对路径
		DirEntry relativePath[2];
		relativePath[0] = DirEntry(1, 1, rootInodeID, "..");
		relativePath[1] = DirEntry(1, 1, rootInodeID, ".");
		loadFile(rootInodeID);
		File& file = getFile(rootInodeID);
		file.getInode().setOwnerPermissions();
		file.getInode().setOtherPemissions();
		file.resizeContent(DIR_SIZE);
		memcpy(file.getContent(), relativePath, DIR_SIZE);
		writeFileBack(rootInodeID);
	}
	loadFile(rootInodeID);
}

Directory Simdisk::openDir(const string& targetPath, Directory dir) {
	if (targetPath.empty()) {
		return dir;
	}
	//如果是./直接进入子文件夹
	else if (targetPath.substr(0, 2) == "./") {
		return openDir(targetPath.substr(2), dir);
	}
	auto pos = targetPath.find('/');
	if (pos == string::npos) {
		if (dir.countDir(targetPath)) {
			DirEntry& target = dir[targetPath];
			if (target.getType()) {
				loadFile(target.getInodeID());
				return Directory(&getFile(target.getInodeID()));
			}
		}
		throw string("dir doesn't exist");
	}
	else if (!pos) {
		return openDir(targetPath.substr(1), Directory(&getFile(superBlock.getRootInode())));
	}

	string dirname = targetPath.substr(0, pos);
	if (dir.countDir(dirname)) {
		DirEntry& target = dir[dirname];
		if (target.getType()) {
			uint16_t targetID = target.getInodeID();
			loadFile(targetID);
			return openDir(targetPath.substr(pos + 1), Directory(&getFile(targetID)));//递归打开子目录
		}
	}
	throw string("dir doesn't exist");
}

void Simdisk::createDir(uint8_t userID, Directory targetDir, string dirname) {
	if (targetDir.countDir(dirname)) {
		throw string("<dir> " + dirname + " has already existed");
	}
	uint16_t targetInodeID = createFile();
	bitmap.setInode(targetInodeID);
	uint16_t parentInodeID = targetDir["."].getInodeID();//目标目录为新建目录的父目录
	//建立相对路径
	DirEntry relativePath[2];
	relativePath[0] = DirEntry(1, 1, parentInodeID, "..");
	relativePath[1] = DirEntry(1, 1, targetInodeID, ".");
	File& file = getFile(targetInodeID);
	file.setOwner(userID);
	file.resizeContent(DIR_SIZE);
	memcpy(file.getContent(), relativePath, DIR_SIZE);
	targetDir.addFile(targetInodeID, dirname, 1);
	writeFileBack(parentInodeID);
	writeFileBack(targetInodeID);
	closeFile(targetInodeID);
}

void Simdisk::deleteSubdir(Directory dir) {
	for (auto& iter : dir.getMap()) {
		if (iter.second.getFilename() == "." || iter.second.getFilename() == "..") {
			continue;
		}
		else if (iter.second.getType()) {
			uint16_t subdirInodeID = iter.second.getInodeID();
			loadFile(subdirInodeID);
			File& subdirFile = getFile(subdirInodeID);
			Directory subdir(&subdirFile);
			deleteSubdir(subdir);
		}
		deleteFile(iter.second.getInodeID());
	}
}

uint16_t Simdisk::createFile() {
	uint16_t targetInodeID = getAvlInodeID();
	superBlock.freeInodeCntDecrement();
	File file(&inodeList[targetInodeID]);
	file.getInode().formatting();
	file.getInode().setUsed();
	map.insert(make_pair(targetInodeID, file));
	return targetInodeID;
}

File& Simdisk::getFile(uint16_t inodeID) {
	return ref(map[inodeID]);
}

void Simdisk::loadFile(uint16_t inodeID) {
	if (!inodeList[inodeID].testUsed()) {
		throw string("File doesn't exist");
	}
	if (!map.count(inodeID)) {
		map.insert(make_pair(inodeID, *new File(&inodeList[inodeID])));//将文件添加到映射
		setFileBlockLists(map[inodeID]);
		setFileContent(map[inodeID]);
	}
}

void Simdisk::writeFileBack(uint16_t inodeID) {
	loadFile(inodeID);
	File& file = map[inodeID];

	//确定文件大小
	int32_t blockNeeds = file.getContentSize() / BLOCKSIZE;
	uint16_t remaining = file.getContentSize() % BLOCKSIZE;
	remaining ? blockNeeds++ : remaining = BLOCKSIZE;
	if (blockNeeds > MAX_FILE_BLOCK_NUM) {
		throw string("Filesize overflow");
	}

	//申请|释放硬盘空间并构建blockLists
	uint32_t blockID = BAD_BLOCK_ID;
	while (blockNeeds > file.getBlockListsSize()) {
		blockID = getAvlBlockID();
		bitmap.setBlock(blockID);
		try {
			superBlock.freeBlockCntDecrement();
		}
		catch (string errorMessage) {
			cout << errorMessage << endl;
		}
		file.insertBlockLists(blockID);
	}
	if (blockNeeds < file.getBlockListsSize()) {
		for (int i = blockNeeds; i < file.getBlockListsSize(); ++i) {
			bitmap.setBlock(file.getBlockLists()[i], 0);
			try {
				superBlock.freeBlockCntIncrement();
			}
			catch (string errorMessage) {
				cout << errorMessage << endl;
			}
		}
		file.resizeBlockLists(blockNeeds);
	}

	//将文件内容写入磁盘块
	for (int i = 0; i < blockNeeds; ++i) {
		i == (blockNeeds - 1) ? writeBlock(file.getBlockLists()[i], file.getContent(i * BLOCKSIZE), remaining)
			: writeBlock(file.getBlockLists()[i], file.getContent(i * BLOCKSIZE));
	}

	//构建i节点的直接块与间接块
	//直接块
	file.getInode().setBlockCnt(blockNeeds);
	file.getInode().setFileSize(file.getContentSize());
	for (uint8_t i = 0; i < min(10, blockNeeds); ++i) {
		file.getInode().setBlockX1(i, file.getBlockLists()[i]);
	}
	blockNeeds -= 10;
	uint32_t blockX2ID = file.getInode().getBlockX2();
	uint32_t blockX3ID = file.getInode().getBlockX3();
	//若未使用间接块
	if (blockNeeds <= 0) {
		if (blockX2ID != BAD_BLOCK_ID) {
			file.getInode().setBlockX2(BAD_BLOCK_ID);
			bitmap.setBlock(blockID, 0);
			superBlock.freeBlockCntIncrement();
		}
		if (blockX3ID != BAD_BLOCK_ID) {
			file.getInode().setBlockX3(BAD_BLOCK_ID);
			bitmap.setBlock(blockID, 0);
			superBlock.freeBlockCntIncrement();
		}
		setFileBlockLists(file);
		return;
	}

	//一次间接块
	if (blockX2ID == BAD_BLOCK_ID) {
		blockX2ID = getAvlBlockID();
		bitmap.setBlock(blockX2ID);
		superBlock.freeBlockCntDecrement();
		file.getInode().setBlockX2(blockX2ID);
	}
	writeBlock(blockX2ID, (char*)&file.getBlockLists()[10], min(BLOCKSIZE, blockNeeds * sizeof(uint32_t)));
	blockNeeds -= BLOCKADDRS_PER_BLOCK;

	//若未使用二次间接块
	if (blockNeeds <= 0) {
		if (blockX3ID != BAD_BLOCK_ID) {
			file.getInode().setBlockX3(BAD_BLOCK_ID);
			bitmap.setBlock(blockX3ID, 0);
			superBlock.freeBlockCntIncrement();
		}
		setFileBlockLists(file);
		return;
	}

	//二次间接块
	vector<uint32_t>blockX3(BLOCKADDRS_PER_BLOCK, BAD_BLOCK_ID);
	if (blockX3ID == BAD_BLOCK_ID) {
		blockX3ID = getAvlBlockID();
		bitmap.setBlock(blockX3ID);
		superBlock.freeBlockCntDecrement();
		file.getInode().setBlockX3(blockX3ID);
	}
	else {
		readBlock((char*)&blockX3[0], blockX3ID);
	}
	uint32_t count = 1;
	for (auto& blockX2ID : blockX3) {
		if (blockX2ID == BAD_BLOCK_ID) {
			blockX2ID = getAvlBlockID();
			bitmap.setBlock(blockX2ID);
			superBlock.freeBlockCntDecrement();
		}
		writeBlock(blockX2ID, (char*)&file.getBlockLists()[10 + count * BLOCKADDRS_PER_BLOCK], min(BLOCKSIZE, blockNeeds * sizeof(uint32_t)));
		blockNeeds -= BLOCKADDRS_PER_BLOCK;
		if (blockNeeds <= 0) {
			break;
		}
		count++;
	}
	writeBlock(blockX3ID, (char*)&blockX3[0]);
	setFileBlockLists(file);
}

void Simdisk::closeFile(uint16_t inodeID) {
	if (map.count(inodeID)) {
		map.erase(inodeID);
	}
}

void Simdisk::deleteFile(uint16_t targetFileInodeID) {
	loadFile(targetFileInodeID);
	File& file = map[targetFileInodeID];
	for (auto& blockID : file.getBlockLists()) {
		bitmap.setBlock(blockID, 0);
		superBlock.freeBlockCntIncrement();
		blockID = BAD_BLOCK_ID;
	}
	writeFileBack(targetFileInodeID);
	inodeList[targetFileInodeID].formatting();
	bitmap.setInode(targetFileInodeID, 0);
	map.erase(targetFileInodeID);
	superBlock.freeInodeCntIncrement();
}

uint16_t Simdisk::openFile(string targetPath, Directory& targetDir) {
	auto pos = targetPath.rfind('/');
	if (pos == string::npos) {
		if (targetDir.countDir(targetPath)) {
			DirEntry targetFile = targetDir[targetPath];
			uint16_t targetFileInodeID = targetFile.getInodeID();
			loadFile(targetFileInodeID);
			return targetFileInodeID;
		}
	}
	else {
		Directory dir = openDir(targetPath.substr(0, pos), targetDir);
		string filename = targetPath.substr(pos + 1);
		if (dir.countDir(filename)) {
			DirEntry& targetFile = dir[filename];
			uint16_t targetFileInodeID = targetFile.getInodeID();
			loadFile(targetFileInodeID);
			return targetFileInodeID;
		}
	}
	throw string("file does not exist");
}

uint32_t Simdisk::getAvlSpace() {
	return superBlock.getFreeBlockCnt() * 1024;
}

void Simdisk::setFileBlockLists(File& file) {
	file.clearBlockLists();
	// 获取直接块
	for (auto block : file.getInode().getBlockX1()) {
		if (block == BAD_BLOCK_ID || block == 0) {
			return;
		}
		file.insertBlockLists(block);
	}

	// 获取一次间接块
	uint32_t blockX2ID = file.getInode().getBlockX2();
	if (blockX2ID == BAD_BLOCK_ID) {
		return;
	}
	array<uint32_t, BLOCKADDRS_PER_BLOCK> blockX2;
	readBlock((char*)&blockX2[0], blockX2ID);
	for (auto block : blockX2) {
		if (block == BAD_BLOCK_ID || block == 0) {
			return;
		}
		file.insertBlockLists(block);
	}

	// 获取二次间接块
	uint32_t blockX3ID = file.getInode().getBlockX3();
	if (blockX3ID == BAD_BLOCK_ID) {
		return;
	}
	array<uint32_t, BLOCKADDRS_PER_BLOCK> blockX3;
	readBlock((char*)&blockX3, blockX3ID); // 获取一次间接块
	for (auto blockX2ID : blockX3) {
		if (blockX2ID == BAD_BLOCK_ID) {
			return;
		}
		array<uint32_t, BLOCKADDRS_PER_BLOCK> blockX1;
		readBlock((char*)&blockX1, blockX2ID); // 获取直接块
		for (auto blockID : blockX1) {
			if (blockID == BAD_BLOCK_ID || blockID == 0) {
				return;
			}
			file.insertBlockLists(blockID);
		}
	}
}

void Simdisk::setFileContent(File& file) {
	uint32_t pos = 0;
	file.resizeContent(file.getInode().getFileSize());
	for (auto blockID : file.getBlockLists()) {
		readBlock(file.getContent(pos), blockID);
		pos += BLOCKSIZE;
	}
}