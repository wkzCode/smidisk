#include "DirEntry.h"

DirEntry::DirEntry() {
	formatting();
}

DirEntry::DirEntry(bool useFlag, bool type, uint16_t inodeID, const string& filename) {
	formatting();
	this->useFlag = useFlag;
	this->type = type;
	this->inodeID = inodeID;
	setFilename(filename);
}

DirEntry::DirEntry(const DirEntry& dirEntry) {
	memcpy(this, &dirEntry, DIR_ENTRY_SIZE);
}

void DirEntry::formatting() {
	memset(this, 0, DIR_ENTRY_SIZE);
}

bool DirEntry::getUseFlag() {
	return useFlag;
}

void DirEntry::setUseFlag(bool useFlag) {
	this->useFlag = useFlag;
}

bool DirEntry::getType() {
	return type;
}

void DirEntry::setType(bool type) {
	this->type = type;
}

uint16_t DirEntry::getInodeID() {
	return inodeID;
}

void DirEntry::setInodeID(uint16_t iNodeID) {
	this->inodeID = iNodeID;
}

const string DirEntry::getFilename() {
	return string(filename);
}

void DirEntry::setFilename(const string& filename) {
	uint8_t len = min(FILENAME_LENGTH - 1, filename.size());
	memcpy(this->filename, filename.c_str(), len);//文件名不能超过指定长度
	this->filename[FILENAME_LENGTH - 1] = '\0';
}