#pragma once
#include "CommonUse.h"

class Inode {
private:
	bitset<32> flags;     //��־λ����0λ��ʾ�Ƿ�ʹ�ã���1��2λ��ʾ�����ߵ�д����Ȩ�ޣ���3��4λ��ʾ�����û��Ķ�дȨ��
	uint16_t id;          //i�ڵ�ı��
	uint8_t ownerID;      //�����ߵ��û����
	uint32_t filesize;    //�ļ���С
	uint32_t blockCnt;    //�ļ�ռ�õĿ�����
	uint32_t blockX1[10]; //ֱ�ӿ�
	uint32_t blockX2;     //һ����ӿ�
	uint32_t blockX3;     //������ӿ�
public:
	Inode();
	Inode(const Inode& inode);
	void formatting();

	bool testUsed();
	void setUsed(bool flag = 1);

	bool testWrite(uint8_t uid);
	bool testRead(uint8_t uid);
	string getWRPermissions();
	void setOwnerPermissions(bool w = 1, bool r = 1);
	void setOtherPemissions(bool w = 1, bool r = 1);

	uint8_t getOwnerID();
	void setOwnerID(uint8_t uid);

	uint32_t getFileSize();
	void setFileSize(uint32_t size);

	uint32_t getBlockCnt();
	void setBlockCnt(uint32_t blockCnt);

	vector<uint32_t> getBlockX1();
	uint32_t getBlockX1(uint32_t pos);
	uint32_t getBlockX2();
	uint32_t getBlockX3();
	void setBlockX1(uint8_t pos, uint32_t blockID);
	void setBlockX2(uint32_t blockID);
	void setBlockX3(uint32_t blockID);
};