//
// Created by GDL on 7/5/18.
//

#include "ext4.h"



typedef struct {
    uint64_t inodeTableLocation;
    uint32_t inodeSize;
}EXT4_Globals;

EXT4_Globals globals;

void seekToSuperBlockField(int vol_fd, off_t offset) {
    lseek(vol_fd, SUPERBLOCK_START + offset, SEEK_SET);
}

void readSuperBlockField(int vol_fd, void* destination, size_t size, off_t offset) {
    seekToSuperBlockField(vol_fd, offset);
    read(vol_fd, destination, size);
}

void seekToGroupDescriptorField(int vol_fd, off_t offset) {
    lseek(vol_fd, GROUP_DESCRIPTOR_START + offset, SEEK_SET);
}

void readGroupDescriptorField(int vol_fd, void* destination, size_t destination_size, off_t  offset){
    seekToGroupDescriptorField(vol_fd, offset);
    read(vol_fd, destination, destination_size);
}

void seekToInodeField(int vol_fd, uint32_t inode_number, off_t offset) {
    lseek(vol_fd, globals.inodeTableLocation + (inode_number - 1) * globals.inodeSize + offset , SEEK_SET);
}

void readInodeField(int vol_fd, uint32_t inode_number, void* destination, size_t destination_size, off_t  offset){
    seekToInodeField(vol_fd, inode_number, offset);
    read(vol_fd, destination, destination_size);
}

void EXT4_init(int vol_fd){

    readSuperBlockField(vol_fd, &(globals.inodeSize), sizeof(globals.inodeSize), INODE_SIZE);

    readGroupDescriptorField(vol_fd, &(globals.inodeTableLocation), sizeof(uint32_t), INODE_TABLE_LOC_HI);
    globals.inodeTableLocation = globals.inodeTableLocation << UINT64_NIBBLE_OFFSET;
    readGroupDescriptorField(vol_fd, &(globals.inodeTableLocation), sizeof(uint32_t), INODE_TABLE_LOC_LO);
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

void traverseExtentTree(int vol_fd, off_t next_jump, ExtentGroup* blocks) {

    // seek to new offset
    lseek(vol_fd, next_jump + ENTRY_COUNT, SEEK_SET);

    // read header
    ExtentHeader header;
    read(vol_fd, &(header.entryCount), sizeof(header.entryCount));
    lseek(vol_fd, DEPTH_OFFSET_FROM_ENTRY_COUNT, SEEK_CUR);
    read(vol_fd, &(header.depth), sizeof(header.depth));
    lseek(vol_fd, EXTENT_HEADER_FINAL_FIELDS, SEEK_CUR);

    if(header.depth == 0){
        // if data process the blocks and add them to the list
        for(int i = 0; i < header.entryCount; i++){
            DataExtent extent;
            read(vol_fd, &(extent.firstBlockOfRange), sizeof(extent.firstBlockOfRange));
            read(vol_fd, &(extent.lengthOfRange), sizeof(extent.lengthOfRange));
            read(vol_fd, &(extent.firstBlockAddress), sizeof(uint16_t));
            extent.firstBlockAddress = extent.firstBlockAddress << UINT64_NIBBLE_OFFSET;
            read(vol_fd, &(extent.firstBlockAddress), sizeof(uint32_t));

            blocks->extents[blocks->count] = extent;
            (blocks->count)++;
        }
    } else {
        // if index recursively call this function with the new address
        for(int i = 1; i <= header.entryCount; i++){
            IndexExtent extent;
            read(vol_fd, &(extent.firstBlockOfBranch), sizeof(extent.firstBlockOfBranch));
            uint32_t nn_lo;
            read(vol_fd, &(nn_lo), sizeof(nn_lo));
            read(vol_fd, &(extent.nextNode), sizeof(extent.nextNode));
            extent.nextNode = extent.nextNode << UINT64_NIBBLE_OFFSET + nn_lo;
            traverseExtentTree(vol_fd, extent.nextNode, blocks);
            lseek(vol_fd, next_jump + EXTENT_HEADER_LEN + i * EXTENT_ENTRY_LEN, SEEK_SET);
        }
    }


}

Inode getInodeInfo(int vol_fd, uint32_t inode_number) {

    Inode inode;

    readInodeField(vol_fd, inode_number, &(inode.file_mode), sizeof(inode.file_mode), FILE_MODE);
    readInodeField(vol_fd, inode_number, &(inode.size), sizeof(uint32_t), SIZE_HI);
    inode.size = inode.size << UINT64_NIBBLE_OFFSET;
    readInodeField(vol_fd, inode_number, &(inode.size), sizeof(uint32_t), SIZE_LO);
    readInodeField(vol_fd, inode_number, &(inode.flags), sizeof(inode.flags), INODE_FLAGS);
    readInodeField(vol_fd, inode_number, &(inode.ctime), sizeof(inode.ctime), CREATION_TIME);
    traverseExtentTree(vol_fd, globals.inodeTableLocation + (inode_number - 1) * globals.inodeSize + EXTENT_TREE_ROOT, &(inode.dataBlocks));

    return inode;
}

void searchInDirectory(){

}

EXT4_FileMetadata EXT4_SearchInRoot(int vol_fd, char* target) {

    EXT4_FileMetadata metadata;


    return metadata;
}