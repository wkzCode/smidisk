#pragma once
#include"FileOp.h"
#include"shell.h"

class Simdisk :public FileOp {
private:
	unordered_map<uint16_t, File> map;                                   //i节点到文件映射
public:
	Simdisk() = delete;
	Simdisk(const Simdisk&) = delete;
	explicit Simdisk(const string vDiskName);

	Directory openDir(const string& targetPath, Directory dir);          //打开目录
	void createDir(uint8_t userID, Directory targetDir, string dirname); //新建目录
	void deleteSubdir(Directory dir);                                    //删除子目录

	uint32_t getAvlSpace();                                              //获取磁盘可用空间

	uint16_t createFile();                                               //新建文件
	File& getFile(uint16_t inodeID);                                     //获取文件
	void loadFile(uint16_t inodeID);                                     //加载文件
	void writeFileBack(uint16_t inodeID);                                //写回文件（blockLists，磁盘块，i节点）
	void closeFile(uint16_t inodeID);                                    //关闭文件
	void deleteFile(uint16_t targetFileInodeID);                         //删除文件
	uint16_t openFile(string targetPath, Directory& targetDir);          //打开文件

	void setFileBlockLists(File& file);                                  //设置文件的磁盘块列表
	void setFileContent(File& file);                                     //设置文件内容
};