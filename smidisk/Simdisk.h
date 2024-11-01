#pragma once
#include"FileOp.h"
#include"shell.h"

class Simdisk :public FileOp {
private:
	unordered_map<uint16_t, File> map;                                   //i�ڵ㵽�ļ�ӳ��
public:
	Simdisk() = delete;
	Simdisk(const Simdisk&) = delete;
	explicit Simdisk(const string vDiskName);

	Directory openDir(const string& targetPath, Directory dir);          //��Ŀ¼
	void createDir(uint8_t userID, Directory targetDir, string dirname); //�½�Ŀ¼
	void deleteSubdir(Directory dir);                                    //ɾ����Ŀ¼

	uint32_t getAvlSpace();                                              //��ȡ���̿��ÿռ�

	uint16_t createFile();                                               //�½��ļ�
	File& getFile(uint16_t inodeID);                                     //��ȡ�ļ�
	void loadFile(uint16_t inodeID);                                     //�����ļ�
	void writeFileBack(uint16_t inodeID);                                //д���ļ���blockLists�����̿飬i�ڵ㣩
	void closeFile(uint16_t inodeID);                                    //�ر��ļ�
	void deleteFile(uint16_t targetFileInodeID);                         //ɾ���ļ�
	uint16_t openFile(string targetPath, Directory& targetDir);          //���ļ�

	void setFileBlockLists(File& file);                                  //�����ļ��Ĵ��̿��б�
	void setFileContent(File& file);                                     //�����ļ�����
};