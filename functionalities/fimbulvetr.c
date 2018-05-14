//
// Created by GDL on 19/4/18.
//

#include <time.h>
#include "fimbulvetr.h"
#include "../fs_utils/common.h"
#include "../fs_utils/fat32.h"
#include "../fs_utils/ext4.h"

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

void showResultsEXT() {

    EXT4_FileMetadata fm = EXT4_SearchInRoot(vol_fd, target);;

    fprintf(stdout, "File Found! Size: %d bytes. Created on %s.", fm.size, fm.cdate);
}

void searchForFileInRootDir(char* volume, char* target) {

    int vol_fd = initVolume(volume);
    uint8_t  fs = detectSupportedFileSystem(vol_fd);

    fprintf(stdout, "Searching '%s' in provided volume...\n\n", target);

    switch(fs) {
        case FAT32:
            showResultsFAT(vol_fd, target);

            return;
        case EXT4:
            showResultsEXT(vol_fd, target);
            return;FAT32_SearchInRoot(vol_fd, target);
    }

}



