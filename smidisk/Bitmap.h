#pragma once
#include "CommonUse.h"

class Bitmap {
private:
	bitset<INODE_CNT> inodeBitmap; //i�ڵ�λͼ
	bitset<BLOCK_CNT> blockBitmap; //���̿�λͼ
public:
	void bitmapFormatting();

	bitset<INODE_CNT>& getInodeBitmap();
	bitset<BLOCK_CNT>& getBlockBitmap();

	bool testInode(uint16_t inodeID);
	bool testBlock(uint32_t blockID);

	void setInode(uint16_t inodeID, bool b = 1);
	void setBlock(uint32_t blockID, bool b = 1);
};