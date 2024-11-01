#pragma once
#include"Bitmap.h"
#include"Inode.h"
#include"SuperBlock.h"

class FileOp {
protected:
	fstream f;
	SuperBlock superBlock;
	Bitmap bitmap;
	vector<Inode>inodeList;
public:
	FileOp() = delete;
	FileOp(const FileOp&) = delete;
	FileOp(const string vDiskName);

	SuperBlock getSuperBlock();
	Inode& getInode(uint16_t inodeID);
	bitset<INODE_CNT>getInodeBitmap();
	bitset<BLOCK_CNT>getBlockBitmap();

	void setInodeBitmap(uint16_t inodeID, bool b = 1);
	bool testInodeBitmap(uint16_t inodeID);

	uint32_t getAvlBlockID();
	uint16_t getAvlInodeID();

	void readBlock(char* block, uint32_t blockID);                               //╤а╢еел©И
	void writeBlock(uint32_t blockID, char* content, uint16_t size = BLOCKSIZE); //п╢╢еел©И

	void diskFormatting();
	void save();                                                                 //╠ё╢Ф╣╫╢еел©И

	virtual ~FileOp();
};