//
// Created by GDL on 7/5/18.
//

#include "ext4.h"


typedef struct {
    uint64_t inodeTableLocation;
    uint32_t inodeSize;
    double blockSize;
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
    uint64_t itl = globals.inodeTableLocation;
    lseek(vol_fd, globals.inodeTableLocation + ((inode_number - 1)) * globals.inodeSize + offset , SEEK_SET);
}

void readInodeField(int vol_fd, uint32_t inode_number, void* destination, size_t destination_size, off_t  offset){
    seekToInodeField(vol_fd, inode_number, offset);
    read(vol_fd, destination, destination_size);
}

void EXT4_init(int vol_fd){

    readSuperBlockField(vol_fd, &(globals.inodeSize), sizeof(globals.inodeSize), INODE_SIZE);
    uint16_t logBlockSize;
    readSuperBlockField(vol_fd, &(logBlockSize), sizeof(logBlockSize), LOG_BLOCK_SIZE);
    globals.blockSize = pow(2, 10 + logBlockSize);

    readGroupDescriptorField(vol_fd, &(globals.inodeTableLocation), sizeof(uint32_t), INODE_TABLE_LOC_HI);
    globals.inodeTableLocation = globals.inodeTableLocation << UINT64_NIBBLE_OFFSET;
    readGroupDescriptorField(vol_fd, &(globals.inodeTableLocation), sizeof(uint32_t), INODE_TABLE_LOC_LO);
    globals.inodeTableLocation = globals.inodeTableLocation * (uint64_t)globals.blockSize;

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
            extent.nextNode = (extent.nextNode << UINT64_NIBBLE_OFFSET) + nn_lo;

            traverseExtentTree(vol_fd, extent.nextNode * (uint64_t)globals.blockSize, blocks);
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
    inode.dataBlocks.count = 0;
    traverseExtentTree(vol_fd, globals.inodeTableLocation + (inode_number - 1) * globals.inodeSize + EXTENT_TREE_ROOT, &(inode.dataBlocks));
    inode.dataBlocks.lastBlockUsedSize = (uint16_t) (inode.size % (uint64_t)globals.blockSize);

    return inode;
}

void readBlock(int vol_fd, uint64_t base_block, uint16_t blockInRange, uint8_t* destination) {
    lseek(vol_fd, ((uint64_t)globals.blockSize * (base_block + blockInRange)), SEEK_SET);
    read(vol_fd, destination, (size_t)globals.blockSize);
    int i = 0;
}

EXT4_File* searchInDirectory(int vol_fd, uint32_t inode_number, char* target){

    Inode directory = getInodeInfo(vol_fd, inode_number);
    Block block = (Block)malloc((size_t) (sizeof(uint8_t) * globals.blockSize));

    for(int i = 0; i < directory.dataBlocks.count; i++) {
        for(uint16_t j = 0; j < directory.dataBlocks.extents[i].lengthOfRange; j++){


            readBlock(vol_fd, directory.dataBlocks.extents[i].firstBlockAddress, j, block);
            int actualPos = 0, initialPos = 0;

            while(initialPos < globals.blockSize){

                DirectoryEntry entry;
                actualPos = initialPos;
                bcopy(&(block[actualPos]), &(entry.target_inode), sizeof(entry.target_inode));
                if(entry.target_inode == INODE_ZERO){
                    break;
                }
                actualPos += sizeof(entry.target_inode);
                bcopy(&(block[actualPos]), &(entry.entryLength), sizeof(entry.entryLength));
                actualPos += sizeof(entry.entryLength);
                bcopy(&(block[actualPos]), &(entry.filenameLength), sizeof(entry.filenameLength));
                actualPos += sizeof(entry.filenameLength);
                bcopy(&(block[actualPos]), &(entry.fileType), sizeof(entry.fileType));
                actualPos += sizeof(entry.fileType);
                bcopy(&(block[actualPos]), entry.filename, sizeof(char) * entry.filenameLength);
                entry.filename[entry.filenameLength] = '\0';
                initialPos += entry.entryLength;

                fprintf(stdout, "entry found: '%s' - len: %d - ", entry.filename, entry.filenameLength);
                fprintf(stdout, "filetype: %d - len_entry: \n", entry.fileType);

                if(entry.fileType == DE_FILE && entry.filename[0] != HIDDEN_FILE_PREFIX && strcmp(entry.filename, target) == 0){
                    EXT4_File* file = (EXT4_File*)malloc(sizeof(EXT4_File));
                    bcopy(entry.filename, file->metadata.filename, EXT4_MAX_FILENAME_LEN);
                    Inode inode = getInodeInfo(vol_fd, entry.target_inode);
                    file->metadata.ctime = inode.ctime;
                    file->metadata.size = inode.size;
                    file->content = inode.dataBlocks;
                    return file;
                }

            }
        }
    }
    free(block);
    return NULL;
}

EXT4_File* searchRecursivelyInDirectory(int vol_fd, uint32_t inode_number, char* target){

    Inode directory = getInodeInfo(vol_fd, inode_number);
    Block block = (Block)malloc((size_t) (sizeof(uint8_t) * globals.blockSize));

    for(int i = 0; i < directory.dataBlocks.count; i++) {
        for(uint16_t j = 0; j < directory.dataBlocks.extents[i].lengthOfRange; j++){


            readBlock(vol_fd, directory.dataBlocks.extents[i].firstBlockAddress, j, block);
            int actualPos = 0, initialPos = 0;

            while(initialPos < globals.blockSize){

                DirectoryEntry entry;
                actualPos = initialPos;
                bcopy(&(block[actualPos]), &(entry.target_inode), sizeof(entry.target_inode));
                if(entry.target_inode == INODE_ZERO){
                    break;
                }
                actualPos += sizeof(entry.target_inode);
                bcopy(&(block[actualPos]), &(entry.entryLength), sizeof(entry.entryLength));
                actualPos += sizeof(entry.entryLength);
                bcopy(&(block[actualPos]), &(entry.filenameLength), sizeof(entry.filenameLength));
                actualPos += sizeof(entry.filenameLength);
                bcopy(&(block[actualPos]), &(entry.fileType), sizeof(entry.fileType));
                actualPos += sizeof(entry.fileType);
                bcopy(&(block[actualPos]), entry.filename, sizeof(char) * entry.filenameLength);
                entry.filename[entry.filenameLength] = '\0';
                initialPos += entry.entryLength;

                fprintf(stdout, "entry found: '%s' - len: %d - ", entry.filename, entry.filenameLength);
                fprintf(stdout, "filetype: %d - len_entry: \n", entry.fileType);

                if(entry.fileType == DE_FILE && entry.filename[0] != HIDDEN_FILE_PREFIX && strcmp(entry.filename, target) == 0){
                    EXT4_File* file = (EXT4_File*)malloc(sizeof(EXT4_File));
                    bcopy(entry.filename, file->metadata.filename, EXT4_MAX_FILENAME_LEN);
                    Inode inode = getInodeInfo(vol_fd, entry.target_inode);
                    file->metadata.ctime = inode.ctime;
                    file->metadata.size = inode.size;
                    file->content = inode.dataBlocks;
                    file->inode = entry.target_inode;
                    return file;
                } else if(entry.fileType == DE_DIRECTORY && entry.filename[0] != HIDDEN_FILE_PREFIX ) {
                    EXT4_File* file = searchRecursivelyInDirectory(vol_fd, entry.target_inode, target);
                    if(file != NULL){
                        return file;
                    }
                }

            }
        }
    }
    free(block);
    return NULL;
}

EXT4_File* EXT4_SearchInRoot(int vol_fd, char* target) {
    return searchInDirectory(vol_fd, ROOT_DIR_INODE, target);
}

EXT4_File* EXT4_SearchInVolume(int vol_fd, char* target) {
    return searchRecursivelyInDirectory(vol_fd, ROOT_DIR_INODE, target);
}

void catEXT4File(int vol_fd, ExtentGroup eg) {

    Block block = (Block)malloc((size_t) (sizeof(uint8_t) * globals.blockSize));

    for(int i = 0; i < eg.count; i++) {
        for(uint16_t j = 0; j < eg.extents[i].lengthOfRange; j++){

            readBlock(vol_fd, eg.extents[i].firstBlockAddress, j, block);
            if(block[0] == '\0') return;
            if(i == eg.count - 1 && j == eg.extents[i].lengthOfRange - 1){
                write(1, block, (size_t) (sizeof(uint8_t) * eg.lastBlockUsedSize));
            } else {
                write(1, block, (size_t) (sizeof(uint8_t) * globals.blockSize));
            }

        }
    }

    write(1, "\n\n", 2);
    free(block);

}

void writeToInodeField(int vol_fd, uint32_t inode_number, void* value, size_t value_size, off_t  offset){
        seekToInodeField(vol_fd, inode_number, offset);
        write(vol_fd, value, value_size);
}

bool EXT4_SetRonlyFlag(int vol_fd, char* target, bool value) {

    EXT4_File* f = EXT4_SearchInVolume(vol_fd, target);
    if(f == NULL){
        return false;
    }

    uint32_t i_mode;
    readInodeField(vol_fd, f->inode, &i_mode, sizeof(i_mode), INODE_FLAGS);
    i_mode = (i_mode & IMMUTABLE_FLAG_MASK) | ((value ? SET_IMMUTABLE : UNSET_IMMUTABLE) << BYTE_NIBBLE_OFFSET);
    writeToInodeField(vol_fd, f->inode, &i_mode, sizeof(i_mode), INODE_FLAGS);
    readInodeField(vol_fd, f->inode, &i_mode, sizeof(i_mode), INODE_FLAGS);
    return true;

}

bool EXT4_SetCreationDate(int vol_fd, char* target, char* value) {

    EXT4_File* f = EXT4_SearchInVolume(vol_fd, target);
    if(f == NULL){
        return false;
    }

    struct tm tm_crDate;
    char* ret = strptime(value, "%d%m%Y", &tm_crDate);

    if(ret == NULL || *ret != '\0') exit(INVALID_DATE_FORMAT);

    uint32_t crDate = (uint32_t) mktime(&tm_crDate);
    writeToInodeField(vol_fd, f->inode, &crDate, sizeof(crDate), CREATION_TIME);
    return true;

}