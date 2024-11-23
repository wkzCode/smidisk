#pragma once
#include "CommonUse.h"

class SuperBlock {
private:
	uint16_t inodeCnt;         //i�ڵ�����
	uint32_t blockCnt;         //���̿�����
	uint16_t magicNum;         //�ļ�ϵͳħ������ʶ�ļ�ϵͳ����
	uint16_t inodeBitmapPos;   //i�ڵ�λͼ��ַ 
	uint32_t blockBitmapPos;   //��λͼ��ַ
	uint16_t firstFreeInodeID; //��һ������i�ڵ�ID
	uint32_t firstFreeBlockID; //��һ�����д��̿�ID
	uint16_t freeInodeCnt;     //����i�ڵ�����
	uint32_t freeBlockCnt;     //���д��̿�����
	uint16_t firstInodePos;    //��һ��i�ڵ��ڴ����ϵ�λ��
	uint32_t firstBlockPos;    //��һ����ʹ�ô��̿��ڴ����ϵ�λ��
	uint16_t rootInode;        //��Ŀ¼i�ڵ�
public:
	void superBlockFormatting(uint16_t inodeBpSize, uint32_t blockBpSize);

	uint16_t getMagicNum() const;
	uint16_t getInodeBitmapPos() const;
	uint32_t getBlockBitmapPos() const;
	uint16_t getFirstFreeInodeID() const;
	uint32_t getFirstFreeBlockID() const;
	uint16_t getFreeInodeCnt() const;
	uint32_t getFreeBlockCnt() const;
	uint16_t getFirstInodePos() const;
	uint32_t getFirstBlockPos() const;
	uint16_t getRootInode() const;

	void setRootInode(uint16_t rootInode);

	void firstFreeInodeIDIncrement(uint16_t pos);
	void firstFreeBlockIDIncrement(uint32_t pos);

	void freeBlockCntIncrement();
	void freeBlockCntDecrement();
	void freeInodeCntIncrement();
	void freeInodeCntDecrement();
};