#include "SuperBlock.h"

void SuperBlock::superBlockFormatting(uint16_t inodeBitmapSize, uint32_t blockBitmapSize) {
	magicNum = MAGIC_NUM;
	inodeCnt = INODE_CNT;
	blockCnt = BLOCK_CNT;
	freeInodeCnt = INODE_CNT;
	freeBlockCnt = BLOCK_CNT;
	firstFreeInodeID = 1;                                     //第一个i节点被根目录占用
	firstFreeBlockID = 0;
	inodeBitmapPos = BLOCKSIZE;                               //1024 1KB
	blockBitmapPos = inodeBitmapPos + inodeBitmapSize;        //9216 9KB
	firstInodePos = blockBitmapPos + blockBitmapSize;         //21504 21KB
	firstBlockPos = firstInodePos + 64 * inodeBitmapSize * 8; //4215808 4117KB
	rootInode = INODE_CNT;
}

uint16_t SuperBlock::getMagicNum() {
	return magicNum;
}

uint16_t SuperBlock::getInodeBitmapPos() {
	return inodeBitmapPos;
}

uint32_t SuperBlock::getBlockBitmapPos() {
	return blockBitmapPos;
}

uint16_t SuperBlock::getFirstFreeInodeID() {
	return firstFreeInodeID;
}

uint32_t SuperBlock::getFirstFreeBlockID() {
	return firstFreeBlockID;
}

uint16_t SuperBlock::getFreeInodeCnt() {
	return freeInodeCnt;
}

uint32_t SuperBlock::getFreeBlockCnt() {
	return freeBlockCnt;
}

uint16_t SuperBlock::getFirstInodePos() {
	return firstInodePos;
}

uint32_t SuperBlock::getFirstBlockPos() {
	return firstBlockPos;
}

uint16_t SuperBlock::getRootInode() {
	return rootInode;
}

void SuperBlock::setRootInode(uint16_t rootInode) {
	this->rootInode = rootInode;
}

//循环的方式找出空闲的i节点
void SuperBlock::firstFreeInodeIDIncrement(uint16_t pos) {
	++firstFreeInodeID;
	firstFreeInodeID %= INODE_CNT;
	if (firstFreeInodeID == pos) {
		throw string("run out of inode");
	}
}

//循环的方式找出空闲的磁盘块
void SuperBlock::firstFreeBlockIDIncrement(uint32_t pos) {
	++firstFreeBlockID;
	firstFreeBlockID %= BLOCK_CNT;
	if (firstFreeBlockID == pos) {
		throw string("run out of block");
	}
}

void SuperBlock::freeBlockCntIncrement() {
	if (freeBlockCnt == blockCnt) {
		throw string("blocks all free");
	}
	++freeBlockCnt;
}

void SuperBlock::freeBlockCntDecrement() {
	if (!freeBlockCnt) {
		throw string("run out of block");
	}
	--freeBlockCnt;
}

void SuperBlock::freeInodeCntIncrement() {
	if (freeInodeCnt == blockCnt) {
		throw string("inodes all free");
	}
	++freeInodeCnt;
}

void SuperBlock::freeInodeCntDecrement() {
	if (!freeInodeCnt) {
		throw string("run out of indoe");
	}
	--freeInodeCnt;
}