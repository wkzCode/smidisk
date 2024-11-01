#include "Directory.h"

Directory::Directory(const Directory& directory)
	:file(directory.getFile()), map(directory.getMap()) {
	file2Map();
}

Directory::Directory(File* file) :file(file) {
	file2Map();
}

File* Directory::getFile() const {
	return file;
}

unordered_map<string, DirEntry> Directory::getMap() const {
	return map;
}

vector<pair<string, DirEntry>> Directory::Map2Vector() {
	return vector<pair<string, DirEntry>>(map.begin(), map.end());
}

bool Directory::countDir(string dirEntryName) {
	return map.count(dirEntryName);
}

DirEntry& Directory::operator[](string dirEntryName) {
	return map[dirEntryName];
}

uint32_t Directory::getMapSize() {
	return map.size();
}

bool Directory::testRead(uint8_t userID) {
	return file->testRead(userID);
}

bool Directory::testWrite(uint8_t userID) {
	return file->testWrite(userID);
}

Directory& Directory::operator=(const Directory& directory) {
	file = directory.getFile();
	map = directory.getMap();
	file2Map();
	return *this;
}

void Directory::file2Map() {
	uint32_t filesize = file->getContentSize();
	vector<DirEntry>dirs(filesize / DIR_ENTRY_SIZE);
	memcpy(&dirs[0], file->getContent(), filesize);
	map.clear();//清空映射
	for (auto dir : dirs) {
		map.insert(make_pair(dir.getFilename(), dir));
	}
}

void Directory::map2File() {
	vector<DirEntry>dirs(map.size());
	transform(map.begin(), map.end(), dirs.begin(), [](const pair<string, DirEntry>& pair) {return pair.second; });
	uint32_t filesize = dirs.size() * DIR_ENTRY_SIZE;
	file->resizeContent(filesize);
	memcpy(file->getContent(), &dirs[0], filesize);
}

bool Directory::addFile(uint16_t id, string filename, bool type) {
	if (map.count(filename)) {
		throw string(filename + "already exists");
	}
	//将新目录项添加到映射
	map.insert(make_pair(filename, DirEntry(1, type, id, filename)));
	map2File();
}

void Directory::deleteFile(const string& filename) {
	if (!map.erase(filename)) {
		throw string("can't delete non-existent file");
	}
	map2File();
}