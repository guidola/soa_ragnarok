//
// Created by GDL on 19/4/18.
//

#include "functionalities.h"


void showFAT32Info(int vol_fd) {

    FAT32Info info = getFAT32Info(vol_fd);

    fprintf(stdout, "Filesystem: FAT32\n\n");

    fprintf(stdout, "System name: %s\n", info.systemName);
    fprintf(stdout, "Sector size: %d\n", info.sectorSize);
    fprintf(stdout, "Sectors per cluster: %d\n", info.sectorsPerCluster);
    fprintf(stdout, "Reserved sectors: %d\n", info.reservedSectors);
    fprintf(stdout, "Number of FATs: %d\n", info.fatsCount);
    fprintf(stdout, "Maximum root entries: %d\n", info.maximumRootEntries);
    fprintf(stdout, "Sectors per FAT: %d\n", info.sectorsPerFat);
    fprintf(stdout, "Label: %s\n", info.label);

}

void showEXT4Info(int vol_fd) {

    EXT4Info info = getEXT4Info(vol_fd);

    fprintf(stdout, "Filesystem: EXT4\n\n");

    fprintf(stdout, "INODE INFO\n");
    fprintf(stdout, "Inode size: %d\n", info.inodeInfo.inodeSize);
    fprintf(stdout, "Number of inodes: %d\n", info.inodeInfo.inodeCount);
    fprintf(stdout, "First inode: %d\n", info.inodeInfo.firstInode);
    fprintf(stdout, "Inodes per group: %d\n", info.inodeInfo.inodesPerGroup);
    fprintf(stdout, "Free inodes: %d\n\n", info.inodeInfo.inodesPerGroup);

    fprintf(stdout, "BLOCK INFO\n");
    fprintf(stdout, "Block size: %0.f\n", info.blockInfo.blockSize);
    fprintf(stdout, "Reserved blocks: %d\n", info.blockInfo.reservedBlocks);
    fprintf(stdout, "Free blocks: %d\n", info.blockInfo.freeBlocks);
    fprintf(stdout, "Total blocks: %d\n", info.blockInfo.totalBlocks);
    fprintf(stdout, "First block: %d\n", info.blockInfo.firstBlock);
    fprintf(stdout, "Blocks per group: %d\n", info.blockInfo.blocksPerGroup);
    fprintf(stdout, "Frags per group: %d\n\n", info.blockInfo.fragsPerGroup);

    fprintf(stdout, "VOLUME INFO\n");
    fprintf(stdout, "Volume name: %s\n", info.volumeInfo.volumeName);
    time_t chtime = (time_t)(info.volumeInfo.timeOfLastCheck);
    fprintf(stdout, "Last check: %s", ctime(&chtime));
    time_t mtime = (time_t)(info.volumeInfo.timeOfLastMount);
    fprintf(stdout, "Last mount: %s", ctime(&mtime));
    time_t wtime = (time_t)(info.volumeInfo.timeOfLastWrite);
    fprintf(stdout, "Last write: %s\n", ctime(&wtime));


}


void showFileSystemInfo(char* volume) {

    int vol_fd = initVolume(volume);
    uint8_t  fs = detectSupportedFileSystem(vol_fd);

    fprintf(stdout, "-------- FILESYSTEM INFORMATION --------\n\n");

    switch(fs) {
        case FAT32:
            showFAT32Info(vol_fd);
            return;
        case EXT4:
            showEXT4Info(vol_fd);
            return;
    }

}

void showResultsFAT(int vol_fd, char* target, bool isRecursive){

    FAT32_File* f;

    if(isRecursive){
        f = FAT32_SearchInVolume(vol_fd, target);
    } else{
        f = FAT32_SearchInRoot(vol_fd, target);
    }


    if(f == NULL){
        fprintf(stdout, "File not Found!");
        free(f);
        return;
    }

    char crtime[11];
    strftime(crtime, 11, "%d/%m/%Y", (f->metadata.ctime));
    fprintf(stdout, "File Found! Size: %u bytes. Created on %s", f->metadata.size, crtime);
    free(f);
}

void showResultsEXT(int vol_fd, char* target, bool isRecursive) {

    EXT4_File* fm;

    if(isRecursive){
        fm = EXT4_SearchInVolume(vol_fd, target);
    } else{
        fm = EXT4_SearchInRoot(vol_fd, target);
    }



    if(fm == NULL){
        fprintf(stdout, "File not Found!");
        free(fm);
        return;
    }

    char crtime[10];
    time_t tt_ctime = (time_t)(fm->metadata.ctime);
    strftime(crtime, 10, "%d/%m/%Y", gmtime(&tt_ctime));
    fprintf(stdout, "File Found! Size: %lu bytes. Created on %s", fm->metadata.size, crtime);
    free(fm);
}

void searchForFileInVolume(char* volume, char* target, bool isRecursive) {

    int vol_fd = initVolume(volume);
    uint8_t  fs = detectSupportedFileSystem(vol_fd);

    fprintf(stdout, "Searching '%s' in provided volume...\n\n", target);

    switch(fs) {
        case FAT32:
            FAT32_init(vol_fd);
            showResultsFAT(vol_fd, target, isRecursive);

            return;
        case EXT4:
            EXT4_init(vol_fd);
            showResultsEXT(vol_fd, target, isRecursive);
            return;
    }

}

void showFileFAT(int vol_fd, char* target){

    FAT32_File* f;

    f = FAT32_SearchInVolume(vol_fd, target);

    if(f == NULL){
        fprintf(stdout, "File not Found!");
        free(f);
        return;
    }

    fprintf(stdout, "File Found! Showing content...\n");
    catFAT32File(vol_fd, f->clusterList, f->metadata.size);
    free(f);

}

void showFileEXT(int vol_fd, char* target) {

    EXT4_File* fm;

    fm = EXT4_SearchInVolume(vol_fd, target);


    if(fm == NULL){
        fprintf(stdout, "File not Found!\n");
        free(fm);
        return;
    }

    fprintf(stdout, "File Found! Showing content...\n");
    catEXT4File(vol_fd, fm->content);

    free(fm);
}

void searchAndShowFileInVolume(char* volume, char* target) {

    int vol_fd = initVolume(volume);
    uint8_t  fs = detectSupportedFileSystem(vol_fd);

    fprintf(stdout, "Searching '%s' in provided volume...\n\n", target);

    switch(fs) {
        case FAT32:
            FAT32_init(vol_fd);
            showFileFAT(vol_fd, target);

            return;
        case EXT4:
            EXT4_init(vol_fd);
            showFileEXT(vol_fd, target);
            return;
    }

}

void setFileRonlyFAT(int vol_fd, char* target, bool value) {

    if(FAT32_SetRonlyFlag(vol_fd, target, value)){
        fprintf(stdout, "RONLY status for '%s' set to %s.\n", target, value ? "true" : "false");
    } else {
        fprintf(stdout, "Could not find target file.\n");
    }

}

void setFileRonlyEXT(int vol_fd, char* target, bool value) {

    if(EXT4_SetRonlyFlag(vol_fd, target, value)){
        fprintf(stdout, "RONLY status for '%s' set to %s.\n", target, value ? "true" : "false");
    } else {
        fprintf(stdout, "Could not find target file.\n");
    }

}

void setFileWriteOnly(char* volume, char* target, bool value) {

    int vol_fd = initVolume(volume);
    uint8_t  fs = detectSupportedFileSystem(vol_fd);

    fprintf(stdout, "Searching R_ONLY target '%s' in provided volume...\n\n", target);

    switch(fs) {
        case FAT32:
            FAT32_init(vol_fd);
            setFileRonlyFAT(vol_fd, target, value);

            return;
        case EXT4:
            EXT4_init(vol_fd);
            setFileRonlyEXT(vol_fd, target, value);
            return;
    }

}

void setFileHidden(char* volume, char* target, bool value) {

    int vol_fd = initVolume(volume);
    uint8_t  fs = detectSupportedFileSystem(vol_fd);

    if(fs != FAT32) {
        fprintf(stdout, "Operation not supported for this filesystem.\n");
        return;
    }

    FAT32_init(vol_fd);
    fprintf(stdout, "Searching HIDDEN target '%s' in provided volume...\n\n", target);

    if(FAT32_SetHiddenFlag(vol_fd, target, value)){
        fprintf(stdout, "Hidden status for '%s' set to %s.\n", target, value ? "true" : "false");
    } else {
        fprintf(stdout, "Could not find target file.\n");
    }

}

void changeCreationDateFAT(int vol_fd, char* target, char* value) {
    if(FAT32_SetCreationDate(vol_fd, target, value)){
        fprintf(stdout, "'%s' creation date set to %s.\n", target, value);
    } else {
        fprintf(stdout, "Could not find target file.\n");
    }
}

void changeCreationDateEXT(int vol_fd, char* target, char* value) {

    if(EXT4_SetCreationDate(vol_fd, target, value)){
        fprintf(stdout, "'%s' creation date set to %s.\n", target, value);
    } else {
        fprintf(stdout, "Could not find target file.\n");
    }

}

void setFileCreationDate(char* volume, char* target, char* value) {

    int vol_fd = initVolume(volume);
    uint8_t  fs = detectSupportedFileSystem(vol_fd);

    fprintf(stdout, "Searching date change target '%s' in provided volume...\n\n", target);

    switch(fs) {
        case FAT32:
            FAT32_init(vol_fd);
            changeCreationDateFAT(vol_fd, target, value);

            return;
        case EXT4:
            EXT4_init(vol_fd);
            changeCreationDateEXT(vol_fd, target, value);
            return;
    }
}

