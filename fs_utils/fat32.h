//
// Created by GDL on 7/5/18.
//

#ifndef SOA_RAGNAROK_FAT32_H
#define SOA_RAGNAROK_FAT32_H

#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <math.h>


#define BOOT_SECTOR_START 0x00

/** BOOT SECTOR OFFSETS **/

#define SYSTEM_NAME             0x03
#define BYTES_PER_LOGIC_SECTOR  0x0B
#define SECTORS_PER_CLUSTER     0x0D
#define RESERVED_SECTORS_COUNT  0x0E
#define FAT_COUNT               0x10
#define MAX_ROOT_DIR_ENTRIES    0x11
#define SECTORS_PER_FAT         0x24

// 2B
#define FS_INFO_SECTOR          0x30

// 1B -> 0x29 to indicate EBPB format
#define EXT_BOOT_SIGN           0x26

// 1B
#define FAT32_EXT_BOOT_SIGN     0x42
#define VOLUME_LABEL            0x47

// 8B -> non existant if 0x26 sign is 0x00
#define FS_TYPE_12_16           0x36
// 8B -> non existant if 0x42 sign is set to 0x28
#define FS_TYPE_32              0x52


/** FAT TYPES **/
#define S_FAT12 "FAT12"
#define S_FAT16 "FAT16"
#define S_FAT32 "FAT32"

#define NONE    0x00
// skipping 0x01-3 cause they are used in extX filesystem IDs.
#define FAT12   0x04
#define FAT16   0x05
#define FAT32   0x06


typedef struct {
    char     systemName[8];
    uint16_t sectorSize;
    uint8_t  sectorsPerCluster;
    uint16_t reservedSectors;
    uint8_t  fatsCount;
    uint16_t maximumRootEntries;
    uint32_t sectorsPerFat;
    char     label[11];
} FAT32Info;


FAT32Info getFAT32Info(int vol_fd);
uint8_t isFAT(int vol_fd);

#endif //SOA_RAGNAROK_FAT32_H