#pragma once
#include<algorithm>
#include <array>
#include <bitset>
#include<cstdint>
#include <cstring>
#include <fstream>
#include<future>
#include <iostream>
#include<set>
#include <sstream>
#include <stdexcept>
#include <string>
#include <thread>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <Windows.h>

using namespace std;

constexpr uint16_t BLOCKSIZE = 1024;//1KB
constexpr uint32_t DISKSIZE = 1024 * 1024 * 100;//100MB=100*1024KB
constexpr uint32_t BAD_BLOCK_ID = 0xffffffff;
static constexpr uint16_t MAGIC_NUM = 0xEF53;//与EXT2/3/4一致
static constexpr uint16_t INODE_CNT = 65535;//2^16-1
static constexpr uint32_t BLOCK_CNT = 98283;//100*1024-4117
static constexpr uint8_t DIR_ENTRY_SIZE = 128;
static constexpr uint16_t DIR_SIZE = 2 * DIR_ENTRY_SIZE;//256
static constexpr uint8_t FILENAME_LENGTH = DIR_ENTRY_SIZE - 2 * sizeof(bool) - sizeof(uint16_t);//124
static constexpr uint16_t SHARED_MEMORY_SIZE = 1024;
static constexpr uint8_t INODE_SIZE = 64;
static constexpr uint16_t BLOCKADDRS_PER_BLOCK = BLOCKSIZE / sizeof(uint32_t);//256
static constexpr uint32_t MAX_FILE_BLOCK_NUM = 10 + BLOCKADDRS_PER_BLOCK + BLOCKADDRS_PER_BLOCK * BLOCKADDRS_PER_BLOCK;//65,802，支持最大文件为65802KB
static constexpr uint32_t MAX_FILESIZE = MAX_FILE_BLOCK_NUM * BLOCKSIZE;

class Bitmap;
class Directory;
class DirEntry;
class File;
class FileOp;
class Inode;
class NamedPipe;
class SuperBlock;
class Shell;
class Simdisk;