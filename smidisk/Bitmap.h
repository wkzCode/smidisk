#pragma once
#include "CommonUse.h"

class Bitmap {
private:
	bitset<INODE_CNT> inodeBitmap; //i节点位图
	bitset<BLOCK_CNT> blockBitmap; //磁盘块位图
public:
	void bitmapFormatting();

	bitset<INODE_CNT>& getInodeBitmap();
	bitset<BLOCK_CNT>& getBlockBitmap();

	bool testInode(uint16_t inodeID);
	bool testBlock(uint32_t blockID);

	void setInode(uint16_t inodeID, bool b = 1);
	void setBlock(uint32_t blockID, bool b = 1);
};