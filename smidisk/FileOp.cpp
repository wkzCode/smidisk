#include "FileOp.h"

FileOp::FileOp(const string vDiskName) {
	inodeList.resize(INODE_CNT);
	f.open(vDiskName, ios::in | ios::out | ios::binary);
	if (!f) {
		throw string("Virtual disc file system has not been established!");
	}
	f.read((char*)&superBlock, sizeof(superBlock));
	if (superBlock.getMagicNum() != MAGIC_NUM) {
		diskFormatting();
		return;
	}

	//��ȡi�ڵ�λͼ
	f.seekg(superBlock.getInodeBitmapPos());
	f.read((char*)&bitmap.getInodeBitmap(), sizeof(bitmap.getInodeBitmap()));

	//��ȡ��λͼ
	f.seekg(superBlock.getBlockBitmapPos());
	f.read((char*)&bitmap.getBlockBitmap(), sizeof(bitmap.getBlockBitmap()));;

	//��ȡi�ڵ��
	f.seekg(superBlock.getFirstInodePos());
	f.read(reinterpret_cast<char*>(inodeList.data()), INODE_CNT * sizeof(Inode));
}

SuperBlock FileOp::getSuperBlock() {
	return ref(superBlock);
}

bitset<INODE_CNT> FileOp::getInodeBitmap() {
	return bitmap.getInodeBitmap();
}

bitset<BLOCK_CNT> FileOp::getBlockBitmap() {
	return bitmap.getBlockBitmap();
}

void FileOp::setInodeBitmap(uint16_t inodeID, bool b) {
	bitmap.setInode(inodeID, b);
}

bool FileOp::testInodeBitmap(uint16_t inodeID) {
	return bitmap.testInode(inodeID);
}

Inode& FileOp::getInode(uint16_t inodeID) {
	return ref(inodeList[inodeID]);
}

uint32_t FileOp::getAvlBlockID() {
	uint32_t pos = superBlock.getFirstFreeBlockID();
	while (bitmap.testBlock(superBlock.getFirstFreeBlockID())) {
		superBlock.firstFreeBlockIDIncrement(pos);
	}
	return superBlock.getFirstFreeBlockID();
}

uint16_t FileOp::getAvlInodeID() {
	uint16_t pos = superBlock.getFirstFreeInodeID();
	while (bitmap.testInode(superBlock.getFirstFreeInodeID())) {
		try {
			superBlock.firstFreeInodeIDIncrement(pos);
		}
		catch (string errorMessage) {
			cout << errorMessage << endl;
		}
	}
	return superBlock.getFirstFreeInodeID();
}

void FileOp::readBlock(char* block, uint32_t blockID) {
	uint32_t pos = superBlock.getFirstBlockPos() + blockID * BLOCKSIZE;
	f.seekg(pos);
	f.read(block, BLOCKSIZE);
}

void FileOp::writeBlock(uint32_t blockID, char* content, uint16_t size) {
	uint32_t pos = superBlock.getFirstBlockPos() + blockID * BLOCKSIZE;
	f.seekp(pos);
	f.write(content, size);
}

void FileOp::diskFormatting() {
	superBlock.superBlockFormatting(sizeof(bitmap.getInodeBitmap()), sizeof(bitmap.getBlockBitmap()));
	bitmap.bitmapFormatting();
	save();
}

void FileOp::save() {
	//���泬����
	f.seekp(ios::beg);
	f.write((char*)&superBlock, sizeof(superBlock));

	//����i�ڵ�λͼ
	f.seekp(superBlock.getInodeBitmapPos());
	f.write((char*)&bitmap.getInodeBitmap(), sizeof(bitmap.getInodeBitmap()));

	//�����λͼ
	f.seekp(superBlock.getBlockBitmapPos());
	f.write((char*)&bitmap.getBlockBitmap(), sizeof(bitmap.getBlockBitmap()));

	//����i�ڵ��
	f.seekp(superBlock.getFirstInodePos());
	f.write(reinterpret_cast<char*>(inodeList.data()), INODE_CNT * sizeof(Inode));
}

FileOp::~FileOp() {
	save();
	f.close();
}