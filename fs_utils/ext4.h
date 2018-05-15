//
// Created by GDL on 7/5/18.
//

#ifndef SOA_RAGNAROK_EXT4_H
#define SOA_RAGNAROK_EXT4_H

#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <math.h>

/** LAYOUT MAIN POSITIONS **/
#define SUPERBLOCK_START        1024
#define GROUP_DESCRIPTOR_START  2048 // padding + superblock_len


/** EXT4 SUPERBLOCK FIELDS OFFSETS IN BYTES **/
#define INODE_COUNT                 0x00
#define BLOCK_COUNT                 0x04
#define RESERVED_BLOCK_COUNT        0x08
#define FREE_BLOCK_COUNT            0x0C
#define FREE_INODE_COUNT            0x10
#define FIRST_BLOCK                 0x14
#define LOG_BLOCK_SIZE              0x18

#define BLOCKS_PER_GROUP            0x20
#define FRAGS_PER_GROUP             0x24
#define INODES_PER_GROUP            0x28
#define LAST_MOUNT_TIME             0x2C
#define LAST_WRITE_TIME             0x30
#define MAGIC_SIGNATURE             0x38
#define LAST_CHECK_TIME             0x40
#define FIRST_NON_RESERVED_INODE    0x54
#define INODE_SIZE                  0x58
#define FEATURE_COMPAT              0x5C
#define FEATURE_INCOMPAT            0x60
#define VOLUME_NAME                 0x78

/** OFFSETS ON GROUP DESCRIPTOR (GD) **/
#define INODE_TABLE_LOC_LO          0x08
#define INODE_TABLE_LOC_HI          0x28

#define UINT64_NIBBLE_OFFSET        0x20

typedef struct {
    uint64_t inodeTableLocation;
}GroupDescriptor;

/** EXT VERSION CODES **/
#define EXT2 0x01
#define EXT3 0x02
#define EXT4 0x03

/** FEATURE_COMPAT/INCOMPAT FLAGS **/
#define HAS_JOURNAL                 0x04
#define USES_EXTENTS                0x40


/** OTHER VALUES **/
# define MAGIC_NUMBER 0xEF53


/** EXT4 INFO STRUCTURES **/
typedef struct {
    uint16_t inodeSize;
    uint32_t inodeCount;
    uint32_t firstInode;
    uint32_t inodesPerGroup;
    uint32_t freeInodes;

} InodeInfo;

typedef struct {
    uint16_t logBlockSize;
    double blockSize;
    uint32_t reservedBlocks;
    uint32_t freeBlocks;
    uint32_t totalBlocks;
    uint32_t firstBlock;
    uint32_t blocksPerGroup;
    uint32_t fragsPerGroup;

} BlockInfo;

typedef struct {
    char    volumeName[16];
    uint32_t timeOfLastCheck;
    uint32_t timeOfLastMount;
    uint32_t timeOfLastWrite;

} VolumeInfo;

typedef struct {
    InodeInfo   inodeInfo;
    BlockInfo   blockInfo;
    VolumeInfo  volumeInfo;

} EXT4Info;


/** EXTENT HEADER OFFSETS **/
#define ENTRY_COUNT                     0x02
#define DEPTH_OFFSET_FROM_ENTRY_COUNT   0x02
#define EXTENT_HEADER_FINAL_FIELDS      0x04

#define EXTENT_HEADER_LEN               0x0C
#define EXTENT_ENTRY_LEN                0x0C

/** EXTENT INDEX NODE OFFSETS **/
#define FIRST_BLOCK_OF_BRANCH   0x00
#define NEXT_NODE_LO            0x04
#define NEXT_NODE_HI            0x08

/** EXTENT DATA NODE OFFSETS **/
#define FIRST_BLOCK_NUM     0x00
#define NUM_OF_BLOCKS       0x04
#define FIRST_BLOCK_HI      0x06
#define FIRST_BLOCK_LO      0x08

typedef struct {
    uint16_t entryCount;
    uint16_t depth;
}ExtentHeader;

typedef struct {
    uint32_t firstBlockOfBranch;
    uint64_t nextNode; //only 48 bits number. Upper 2B will be 0x00
}IndexExtent;

typedef struct {
    uint32_t firstBlockOfRange;
    uint16_t lengthOfRange;
    uint64_t firstBlockAddress;
}DataExtent;

#define MAX_RANGES 1000
typedef struct {
    DataExtent extents[MAX_RANGES];
    int count;
}ExtentGroup;

/** INODE field offsets **/

#define FILE_MODE           0x00
#define SIZE_LO             0x04
#define INODE_FLAGS         0x20
#define EXTENT_TREE_ROOT    0x28
#define SIZE_HI             0x6C
#define CREATION_TIME       0x90

typedef struct {

    uint16_t file_mode;
    uint64_t size; // composed from i_size_lo(32) & i_size_hi(32)
    uint32_t flags;
    ExtentGroup dataBlocks;
    uint32_t ctime;
}Inode;

typedef struct {
    uint64_t size;
    uint32_t ctime;
}EXT4_FileMetadata;

void seekToSuperBlockField(int vol_fd, off_t offset);
bool isExt(int vol_fd);
uint8_t getExtVersion(int vol_fd);
EXT4Info getEXT4Info(int vol_fd);
EXT4_FileMetadata EXT4_SearchInRoot(int vol_fd, char* target);
void EXT4_init(int vol_fd);

#endif //SOA_RAGNAROK_EXT4_H
