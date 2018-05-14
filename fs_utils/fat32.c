//
// Created by GDL on 7/5/18.
//

#include <strings.h>
#include "fat32.h"


void seekToBootSectorField(int vol_fd, off_t offset) {
    lseek(vol_fd, BOOT_SECTOR_START + offset, SEEK_SET);
}

void readBootSectorField(int vol_fd, void* destination, size_t size, off_t offset) {
    seekToBootSectorField(vol_fd, offset);
    read(vol_fd, destination, size);
}


uint8_t isFAT(int vol_fd) {

    char type[8];
    readBootSectorField(vol_fd, type, sizeof(char) * 8, FS_TYPE_12_16);

    if(bcmp(type, S_FAT12, 5) == 0){
        return FAT12;
    } else if(bcmp(type, S_FAT16, 5) == 0){
        return FAT16;
    }

    readBootSectorField(vol_fd, type, sizeof(char) * 8, FS_TYPE_32);
    if(bcmp(type, S_FAT32, 5) == 0){
        return FAT32;
    }

    return NONE;
}


// execute only when detected a EXT type volume otherwise returned info might be invalid
FAT32Info getFAT32Info(int vol_fd) {

    FAT32Info info;

    readBootSectorField(vol_fd, &(info.sectorSize), sizeof(info.sectorSize), BYTES_PER_LOGIC_SECTOR);
    readBootSectorField(vol_fd, &(info.sectorsPerCluster), sizeof(info.sectorsPerCluster), SECTORS_PER_CLUSTER);
    readBootSectorField(vol_fd, &(info.fatsCount), sizeof(info.fatsCount), FAT_COUNT);
    readBootSectorField(vol_fd, &(info.reservedSectors), sizeof(info.reservedSectors), RESERVED_SECTORS_COUNT);

    char sign;
    readBootSectorField(vol_fd, &(sign), sizeof(sign), FAT32_EXT_BOOT_SIGN);
    if(sign != 0x00) {
        readBootSectorField(vol_fd, info.label, sizeof(char) * 11, VOLUME_LABEL);
    } else {
        strcpy(info.label, "-");
    }
    readBootSectorField(vol_fd, &(info.maximumRootEntries), sizeof(info.maximumRootEntries), MAX_ROOT_DIR_ENTRIES);
    readBootSectorField(vol_fd, &(info.sectorsPerFat), sizeof(info.sectorsPerFat), SECTORS_PER_FAT);
    readBootSectorField(vol_fd, &(info.systemName), sizeof(char) * 8, SYSTEM_NAME);

    return info;
}


