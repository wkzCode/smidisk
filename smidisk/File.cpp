#include "File.h"

File::File() {
	inode = nullptr;
}

File::File(const File& file)
	: inode(file.inode), blockLists(file.blockLists), content(file.content) {}

File::File(Inode* inode)
	:inode(inode), blockLists(), content(inode->getBlockCnt()* BLOCKSIZE) {}

Inode& File::getInode() {
	return ref(*inode);
}

bool File::testRead(uint8_t userID) {
	return inode->testRead(userID);
}

bool File::testWrite(uint8_t userID) {
	return inode->testWrite(userID);
}

void File::setOwner(uint8_t userID, bool w, bool r) {
	inode->setOwnerID(userID);
	inode->setOwnerPermissions(w, r);
}

uint32_t File::getContentSize() {
	return content.size();
}

char* File::getContent(uint32_t pos) {
	return &content[pos];
}

void File::resizeContent(uint32_t newSize) {
	content.resize(newSize);
}

void File::clearBlockLists() {
	blockLists.clear();
}

void File::insertBlockLists(uint32_t blockID) {
	blockLists.emplace_back(blockID);
}

vector<uint32_t> File::getBlockLists() {
	return blockLists;
}

uint32_t File::getBlockListsSize() {
	return blockLists.size();
}

void File::resizeBlockLists(uint32_t newSize) {
	blockLists.resize(newSize);
}