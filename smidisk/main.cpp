#include"simdisk.h"

void userShell(Simdisk* simdisk, NamedPipe* namedPipe, CRITICAL_SECTION& semaphore, uint8_t userID);

void outputError(string errorMessage);

int main() {
	Simdisk* simdisk = NULL;
	bool b = 1;

	//创建|连接磁盘文件
	while (b) {
		try {
			simdisk = new Simdisk("vDiskFileSystem.disc");
			b = 0;
			cout << "Connect to virtual disc successfully!\n";
		}
		catch (string errorMessage) {
			cout << errorMessage << "\nPlease wait for initialization!\n";
			fstream vDisk("vDiskFileSystem.disc", ios::out | ios::binary);
			vDisk.seekp(DISKSIZE - 1);
			vDisk.write("\0", 1);
			vDisk.close();
		}
	}

	//用于前端登录的命名管道
	NamedPipe* namedPipe = NULL;
	try {
		namedPipe = new NamedPipe("\\\\.\\pipe\\NamedPipe");
	}
	catch (const string errorMessage) {
		outputError(errorMessage);
	}

	//初始化信号量，用于管理共享内存
	CRITICAL_SECTION semaphore;
	InitializeCriticalSection(&semaphore);

	unordered_set<uint8_t>usersID;
	vector<thread>t;
	string userID = namedPipe->read();
	namedPipe->write("");
	namedPipe->~NamedPipe();

	usersID.insert(atoi(userID.c_str()));

	//用于执行指令的命名管道
	NamedPipe* namedPipeUser1 = NULL;
	string pipeName = "\\\\.\\pipe\\NamedPipe" + userID;
	try {
		namedPipeUser1 = new NamedPipe(pipeName);
	}
	catch (const string errorMessage) {
		outputError(errorMessage);
	}
	t.emplace_back(userShell, simdisk, namedPipeUser1, ref(semaphore), atoi(userID.c_str()));//创建线程与前端交互

	while (usersID.size() > 0) {
		//用于前端登录的命名管道
		try {
			namedPipe = new NamedPipe("\\\\.\\pipe\\NamedPipe");
		}
		catch (const string errorMessage) {
			outputError(errorMessage);
		}
		userID = namedPipe->read();

		if (userID.size()) {
			if (usersID.find(atoi(userID.c_str())) != usersID.end()) {
				namedPipe->write("user already login");
				continue;
			}
			else {
				namedPipe->write("");
			}
			namedPipe->~NamedPipe();
			usersID.insert(atoi(userID.c_str()));

			//用于执行指令的命名管道，管道名以用户ID区分
			NamedPipe* namedPipeUser = NULL;
			string pipeName = "\\\\.\\pipe\\NamedPipe" + userID;
			try {
				namedPipeUser = new NamedPipe(pipeName);
			}
			catch (const string errorMessage) {
				outputError(errorMessage);
			}
			t.emplace_back(userShell, simdisk, namedPipeUser, ref(semaphore), atoi(userID.c_str()));//创建线程与前端交互
		}
	}

	//单前端连接代码
	//Shell shell(*simdisk, *namedPipe, 0);
	//while (shell.getStatus()) {
	//	try {
	//		shell.commandLine(shell.getBuffer(namedPipe->read()));
	//	}
	//	catch (const string errorMessage) {
	//		shell.outputError(errorMessage);
	//	}
	//}

	return 0;
}

void userShell(Simdisk* simdisk, NamedPipe* namedPipe, CRITICAL_SECTION& semaphore, uint8_t userID) {
	Shell shell(*simdisk, *namedPipe, userID);
	while (shell.getStatus()) {
		try {
			shell.commandLine(shell.getBuffer(namedPipe->read()), semaphore);
		}
		catch (const string errorMessage) {
			shell.outputError(errorMessage);
		}
	}
}

void outputError(string errorMessage) {
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);                                      // 获取控制台句柄
	SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_INTENSITY);               // 设置文本颜色为红色 (FOREGROUND_RED)
	cerr << errorMessage << endl;
	SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE); // 重置为默认颜色
}
