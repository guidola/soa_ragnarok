//
// Created by GDL on 7/5/18.
//

#include "ext4.h"


void seekToSuperBlock(int vol_fd) {
    lseek(vol_fd, SUPERBLOCK_START, SEEK_SET);

}

void seekToSuperBlockField(int vol_fd, off_t offset) {
    lseek(vol_fd, SUPERBLOCK_START + offset, SEEK_SET);
}

void readSuperBlockField(int vol_fd, void* destination, size_t size, off_t offset) {
    seekToSuperBlockField(vol_fd, offset);
    read(vol_fd, destination, size);
}


bool isExt(int vol_fd) {

    uint16_t m_sign;
    readSuperBlockField(vol_fd, &m_sign, sizeof(m_sign), MAGIC_SIGNATURE);

    if(m_sign == MAGIC_NUMBER) return true;

    return false;
}

// execute only when detected a EXT type volume otherwise returned info might be invalid
uint8_t getExtVersion(int vol_fd) {

    uint32_t f_compat;
    uint32_t f_incompat;
    readSuperBlockField(vol_fd, &f_compat, sizeof(f_compat), FEATURE_COMPAT);
    readSuperBlockField(vol_fd, &f_incompat, sizeof(f_incompat), FEATURE_INCOMPAT);

    if(f_incompat & USES_EXTENTS){
        return EXT4;
    }

    switch(f_compat & HAS_JOURNAL) {
        case HAS_JOURNAL:
            return EXT3;
        default:
            return EXT2;
    }

}

// execute only when detected a EXT type volume otherwise returned info might be invalid
EXT4Info getEXT4Info(int vol_fd) {

    EXT4Info info;

    readSuperBlockField(vol_fd, &(info.inodeInfo.firstInode), sizeof(info.inodeInfo.firstInode), FIRST_NON_RESERVED_INODE);
    readSuperBlockField(vol_fd, &(info.inodeInfo.freeInodes), sizeof(info.inodeInfo.freeInodes), FREE_INODE_COUNT);
    readSuperBlockField(vol_fd, &(info.inodeInfo.inodeCount), sizeof(info.inodeInfo.inodeCount), INODE_COUNT);
    readSuperBlockField(vol_fd, &(info.inodeInfo.inodeSize), sizeof(info.inodeInfo.inodeSize), INODE_SIZE);
    readSuperBlockField(vol_fd, &(info.inodeInfo.inodesPerGroup), sizeof(info.inodeInfo.inodesPerGroup), INODES_PER_GROUP);

    readSuperBlockField(vol_fd, &(info.blockInfo.logBlockSize), sizeof(info.blockInfo.logBlockSize), LOG_BLOCK_SIZE);
    readSuperBlockField(vol_fd, &(info.blockInfo.blocksPerGroup), sizeof(info.blockInfo.blocksPerGroup), BLOCKS_PER_GROUP);
    readSuperBlockField(vol_fd, &(info.blockInfo.firstBlock), sizeof(info.blockInfo.firstBlock), FIRST_BLOCK);
    readSuperBlockField(vol_fd, &(info.blockInfo.fragsPerGroup), sizeof(info.blockInfo.fragsPerGroup), FRAGS_PER_GROUP);
    readSuperBlockField(vol_fd, &(info.blockInfo.freeBlocks), sizeof(info.blockInfo.freeBlocks), FREE_BLOCK_COUNT);
    readSuperBlockField(vol_fd, &(info.blockInfo.reservedBlocks), sizeof(info.blockInfo.reservedBlocks), RESERVED_BLOCK_COUNT);
    readSuperBlockField(vol_fd, &(info.blockInfo.totalBlocks), sizeof(info.blockInfo.totalBlocks), BLOCK_COUNT);

    readSuperBlockField(vol_fd, &(info.volumeInfo.timeOfLastCheck), sizeof(info.volumeInfo.timeOfLastCheck), LAST_CHECK_TIME);
    readSuperBlockField(vol_fd, &(info.volumeInfo.timeOfLastMount), sizeof(info.volumeInfo.timeOfLastMount), LAST_MOUNT_TIME);
    readSuperBlockField(vol_fd, &(info.volumeInfo.timeOfLastWrite), sizeof(info.volumeInfo.timeOfLastWrite), LAST_WRITE_TIME);
    readSuperBlockField(vol_fd, info.volumeInfo.volumeName, 16 * sizeof(char), VOLUME_NAME);

    //calculate non-log block size
    info.blockInfo.blockSize = pow(2, 10 + info.blockInfo.logBlockSize);

    return info;
}

EXT4_FileMetadata EXT4_SearchInRoot(int vol_fd, char* target) {


}