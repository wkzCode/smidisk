#pragma once
#include "CommonUse.h"

class DirEntry {
protected:
	bool useFlag;                   //使用标志位
	bool type;                      //目录标志位：1为目录，0为文件
	uint16_t inodeID;               //对应i节点号
	char filename[FILENAME_LENGTH]; //文件名
public:
	DirEntry();
	DirEntry(bool useFlag, bool type, uint16_t inodeID, const string& filename);
	DirEntry(const DirEntry& dirEntry);

	void formatting();

	bool getUseFlag() const;
	void setUseFlag(bool useFlag);

	bool getType() const;
	void setType(bool type);

	uint16_t getInodeID() const;
	void setInodeID(uint16_t iNodeID);

	const string getFilename();
	void setFilename(const string& filename);
};