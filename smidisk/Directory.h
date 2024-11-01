#pragma once
#include"DirEntry.h"
#include"File.h"

class Directory {
private:
	unordered_map<string, DirEntry> map;         //文件名到目录项映射
	File* file;
public:
	Directory() = delete;
	Directory(const Directory& directory);
	explicit Directory(File* file);

	File* getFile() const;
	unordered_map<string, DirEntry> getMap() const;
	vector<pair<string, DirEntry>> Map2Vector(); //将映射转换为vector数组
	bool countDir(string dirEntryName);
	DirEntry& operator[](string dirEntryName);   //获取对应目录项
	uint32_t getMapSize();

	bool testRead(uint8_t userID);
	bool testWrite(uint8_t userID);

	Directory& operator=(const Directory& directory);
	void file2Map();                             //通过文件重构映射
	void map2File();                             //通过映射重构文件
	bool addFile(uint16_t id, string filename, bool isDir = 0);
	void deleteFile(const string& filename);
};