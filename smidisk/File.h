#pragma once
#include"CommonUse.h"
#include"FileOp.h"
#include"Inode.h"

class File {
private:
	Inode* inode;                                          //�ļ���Ӧ��i�ڵ�
	vector<uint32_t> blockLists;                           //�ļ��Ŀ���б�
	vector<char> content;                                  //�ļ���ʵ������
public:
	File();
	File(const File& file);
	explicit File(Inode* inode);

	Inode& getInode();

	bool testRead(uint8_t userID);
	bool testWrite(uint8_t userID);

	void setOwner(uint8_t userID, bool w = 1, bool r = 1); //����ӵ���ߵ�ID�Ͷ�дȨ��

	uint32_t getContentSize();
	char* getContent(uint32_t pos = 0);
	void resizeContent(uint32_t newSize);

	void clearBlockLists();
	void insertBlockLists(uint32_t blockID);
	vector<uint32_t> getBlockLists();
	uint32_t getBlockListsSize();
	void resizeBlockLists(uint32_t newSize);
};