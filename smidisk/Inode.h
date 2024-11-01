#pragma once
#include "CommonUse.h"

class Inode {
private:
	bitset<32> flags;     //标志位，第0位表示是否使用；第1、2位表示创建者的写、读权限；第3、4位表示其他用户的读写权限
	uint16_t id;          //i节点的编号
	uint8_t ownerID;      //所有者的用户编号
	uint32_t filesize;    //文件大小
	uint32_t blockCnt;    //文件占用的块数量
	uint32_t blockX1[10]; //直接块
	uint32_t blockX2;     //一级间接块
	uint32_t blockX3;     //二级间接块
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