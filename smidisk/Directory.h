#pragma once
#include"DirEntry.h"
#include"File.h"

class Directory {
private:
	unordered_map<string, DirEntry> map;         //�ļ�����Ŀ¼��ӳ��
	File* file;
public:
	Directory() = delete;
	Directory(const Directory& directory);
	explicit Directory(File* file);

	File* getFile() const;
	unordered_map<string, DirEntry> getMap() const;
	vector<pair<string, DirEntry>> Map2Vector(); //��ӳ��ת��Ϊvector����
	bool countDir(string dirEntryName);
	DirEntry& operator[](string dirEntryName);   //��ȡ��ӦĿ¼��
	uint32_t getMapSize();

	bool testRead(uint8_t userID);
	bool testWrite(uint8_t userID);

	Directory& operator=(const Directory& directory);
	void file2Map();                             //ͨ���ļ��ع�ӳ��
	void map2File();                             //ͨ��ӳ���ع��ļ�
	bool addFile(uint16_t id, string filename, bool isDir = 0);
	void deleteFile(const string& filename);
};