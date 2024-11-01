#pragma once
#include "CommonUse.h"

class SuperBlock {
private:
	uint16_t inodeCnt;         //i节点数量
	uint32_t blockCnt;         //磁盘块数量
	uint16_t magicNum;         //文件系统魔数，标识文件系统类型
	uint16_t inodeBitmapPos;   //i节点位图地址 
	uint32_t blockBitmapPos;   //块位图地址
	uint16_t firstFreeInodeID; //第一个空闲i节点ID
	uint32_t firstFreeBlockID; //第一个空闲磁盘块ID
	uint16_t freeInodeCnt;     //空闲i节点数量
	uint32_t freeBlockCnt;     //空闲磁盘块数量
	uint16_t firstInodePos;    //第一个i节点在磁盘上的位置
	uint32_t firstBlockPos;    //第一个可使用磁盘块在磁盘上的位置
	uint16_t rootInode;        //根目录i节点
public:
	void superBlockFormatting(uint16_t inodeBpSize, uint32_t blockBpSize);

	uint16_t getMagicNum();
	uint16_t getInodeBitmapPos();
	uint32_t getBlockBitmapPos();
	uint16_t getFirstFreeInodeID();
	uint32_t getFirstFreeBlockID();
	uint16_t getFreeInodeCnt();
	uint32_t getFreeBlockCnt();
	uint16_t getFirstInodePos();
	uint32_t getFirstBlockPos();
	uint16_t getRootInode();

	void setRootInode(uint16_t rootInode);

	void firstFreeInodeIDIncrement(uint16_t pos);
	void firstFreeBlockIDIncrement(uint32_t pos);

	void freeBlockCntIncrement();
	void freeBlockCntDecrement();
	void freeInodeCntIncrement();
	void freeInodeCntDecrement();
};