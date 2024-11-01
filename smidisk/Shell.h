#pragma once
#include"Directory.h"
#include"NamedPipe.h"
#include"simdisk.h"

class Shell {
private:
	Simdisk& simdisk;
	Directory currentDir;
	NamedPipe& namedPipe;

	uint8_t userID;
	bool usedFlag;

	string currentDirname;

	ostringstream oss;
	istringstream iss;

	multiset<uint16_t>currenInodeSet;
public:
	Shell() = delete;
	Shell(Simdisk& simdisk, NamedPipe& pipe, uint8_t userID = 0);

	bool getStatus();
	string read();

	void commandLine(istringstream iss, CRITICAL_SECTION& semaphore);//进行命令行相关操作

	void showInfoHelp();
	void showInfo();
	void cd(string userInput, CRITICAL_SECTION& semaphore);
	void showDir(Directory targetDir, uint32_t count);
	void showDirSorted(Directory targetDir, uint32_t count);
	void deleteDir(string userInput, Directory targetDir, uint16_t targetDirInodeID, CRITICAL_SECTION& semaphore);
	void newfile(Directory targetDir, string filename, CRITICAL_SECTION& semaphore, string content = "");
	void host2simdisk(string filePath, string targetPath, CRITICAL_SECTION& semaphore);
	void simdisk2host(string filePath, string targetPath, CRITICAL_SECTION& semaphore);
	void simdisk2simdisk(string filePath, string targetPath, CRITICAL_SECTION& semaphore);
	void del(string targetPath, CRITICAL_SECTION& semaphore);
	void check(CRITICAL_SECTION& semaphore);
	void closeShell(CRITICAL_SECTION& semaphore);

	void ossSend();
	istringstream getBuffer(string message);

	void outputError(string message);
};