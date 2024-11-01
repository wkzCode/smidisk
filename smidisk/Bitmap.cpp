#include "Bitmap.h"

void Bitmap::bitmapFormatting() {
	inodeBitmap.set(0);
}

bitset<INODE_CNT>& Bitmap::getInodeBitmap() {
	return inodeBitmap;
}

bitset<BLOCK_CNT>& Bitmap::getBlockBitmap() {
	return blockBitmap;
}

bool Bitmap::testInode(uint16_t inodeID) {
	return inodeBitmap.test(inodeID);
}

bool Bitmap::testBlock(uint32_t blockID) {
	return blockBitmap.test(blockID);
}

void Bitmap::setInode(uint16_t inodeID, bool b) {
	inodeBitmap.set(inodeID, b);
}

void Bitmap::setBlock(uint32_t blockID, bool b) {
	blockBitmap.set(blockID, b);
}