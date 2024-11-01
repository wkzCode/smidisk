#include "Inode.h"

Inode::Inode() {
	formatting();
}

Inode::Inode(const Inode& inode) {
	memcpy(this, &inode, INODE_SIZE);
}

void Inode::formatting() {
	memset(this, 0, INODE_SIZE);
	memset(this->blockX1, 0xff, 12 * sizeof(uint32_t));//ÐÞ¸ÄblockX1¡¢blockX2¡¢blockX3
}

bool Inode::testUsed() {
	return flags[0];
}

void Inode::setUsed(bool flag) {
	flags.set(0, flag);
}

bool Inode::testWrite(uint8_t uid) {
	return (ownerID == uid) ? flags.test(1) : flags.test(3);
}

bool Inode::testRead(uint8_t uid) {
	return (ownerID == uid) ? flags.test(2) : flags.test(4);;
}

string Inode::getWRPermissions() {
	string permissions;
	for (int i = 1; i <= 4; ++i) {
		permissions.append(flags.test(i) ? (i % 2 ? "w" : "r") : "-");
	}
	return permissions;
}

void Inode::setOwnerPermissions(bool w, bool r) {
	flags.set(1, w);
	flags.set(2, r);
}

void Inode::setOtherPemissions(bool w, bool r) {
	flags.set(3, w);
	flags.set(4, r);
}

uint8_t Inode::getOwnerID() {
	return ownerID;
}

void Inode::setOwnerID(uint8_t uid) {
	ownerID = uid;
}

uint32_t Inode::getFileSize() {
	return filesize;
}

void Inode::setFileSize(uint32_t size) {
	filesize = size;
}

uint32_t Inode::getBlockCnt() {
	return blockCnt;
}

void Inode::setBlockCnt(uint32_t blockCnt) {
	this->blockCnt = blockCnt;
}

vector<uint32_t> Inode::getBlockX1() {
	return vector<uint32_t>(blockX1, blockX1 + 10);
}

uint32_t Inode::getBlockX1(uint32_t pos) {
	return blockX1[pos];
}

uint32_t Inode::getBlockX2() {
	return blockX2;
}

uint32_t Inode::getBlockX3() {
	return blockX3;
}

void Inode::setBlockX1(uint8_t pos, uint32_t blockID) {
	blockX1[pos] = blockID;
}

void Inode::setBlockX2(uint32_t blockID) {
	blockX2 = blockID;
}

void Inode::setBlockX3(uint32_t blockID) {
	blockX3 = blockID;
}