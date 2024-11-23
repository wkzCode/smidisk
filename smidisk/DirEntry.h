#pragma once
#include "CommonUse.h"

class DirEntry {
protected:
	bool useFlag;                   //ʹ�ñ�־λ
	bool type;                      //Ŀ¼��־λ��1ΪĿ¼��0Ϊ�ļ�
	uint16_t inodeID;               //��Ӧi�ڵ��
	char filename[FILENAME_LENGTH]; //�ļ���
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