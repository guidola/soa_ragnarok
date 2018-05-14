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
#define SUPERBLOCK_START 1024


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


typedef struct {
    // file metadata at inode entry
}EXT4_FileMetadata;


void seekToSuperBlock(int vol_fd);
void seekToSuperBlockField(int vol_fd, off_t offset);
bool isExt(int vol_fd);
uint8_t getExtVersion(int vol_fd);
EXT4Info getEXT4Info(int vol_fd);
EXT4_FileMetadata EXT4_SearchInRoot(int vol_fd, char* target);

#endif //SOA_RAGNAROK_EXT4_H
