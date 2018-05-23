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
#define ROOT_DIR_START_CLUSTER  0x2C

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
    char     label[12];
} FAT32Info;

/** FAT32 FAT entry OFFSETS **/
// bigger than the threshold indicate EOC
#define EOC_FLAG_THRESHOLD 0x0FFFFFF7
#define FAT32_ENTRY_SIZE 0x04
#define FAT32_ENTRY_MASK 0x0FFFFFFF
#define CLUSTER_LIST_REALLOC_SIZE 1000

#define FAT32_MAX_FILENAME_LEN 261 //TODO change this to the proper value

typedef struct{
    uint32_t* clusters;
    uint64_t  count;
    uint64_t size;
}ClusterList;

/** FAT32 directory entry OFFSETS **/
#define ERASED_FILE_FLAG_VALUE 0xE5
#define LAST_ENTRY_FLAG_VALUE 0x00

#define SHORT_FILE_NAME 0x00
#define SHORT_FILE_NAME_SIZE 0x08
#define SHORT_FILE_EXT 0x08
#define SHORT_FILE_EXT_LEN 0x03
#define FILE_ATTRS 0x0B //1 contains readOnly & hidden flags
#define SUBDIRECTORY_ATTR_MASK 0x10
#define VOLUME_LABEL_ATTR_MASK 0x08
#define HIDDEN_ATTR_MASK 0x02
#define HIDDEN_ATTR_CLEAR 0xFD
#define HIDDEN_ATTR_OFFSET 0x01
#define RONLY_ATTR_MASK 0x01
#define FLAG_UP 0x01
#define FLAG_DOWN 0x00
#define RONLY_ATTR_CLEAR 0xFE

#define CREATION_TIME_FINE 0x0D
#define CREATION_TIME_BASE 0x0E
#define CTIME_BASE_MASK_HOURS 0xF800
#define HOURS_OFFSET 0x0B
#define CTIME_BASE_MASK_MINUTES 0x07E0
#define MINUTES_OFFSET 0x05
#define CTIME_BASE_MASK_SECONDS 0x001F
#define CDATE_BASE 0x10
#define CDATE_BASE_MASK_YEAR 0xFE00
#define YEAR_OFFSET 0x09
#define CDATE_BASE_MASK_MONTH 0x01E0
#define MONTH_OFFSET 0x05
#define CDATE_BASE_MASK_DAY 0x001F
#define BASE_YEAR 80

#define FIRST_CLUSTER_HI 0x14
#define FIRST_CLUSTER_LO 0x1A
#define FILE_SIZE 0x1C

#define DIR_ENTRY_SIZE 0x20

#define VFAT_LONGNAME_ENTRY_FLAG 0x0F //if file_attrs got this value (masked with 0x0F)
#define VFAT_SN 0x00
#define VFAT_SN_FL_MASK 0x40
#define VFAT_SN_VAL_MASK 0x1F
#define VFAT_SN_FIRST 0x01
#define VFAT_NAME_SLOT1 0x01
#define VFAT_NAME_SLOT1_SIZE 0x0A
#define VFAT_NAME_SLOT2 0x0E
#define VFAT_NAME_SLOT2_SIZE 0x0C
#define VFAT_NAME_SLOT3 0x1C
#define VFAT_NAME_SLOT3_SIZE 0x04
#define NAME_BYTES_PER_VFAT_ENTRY 0x1A

#define UINT32_NIBBLE_BITS 16

typedef uint8_t* Cluster;

typedef struct {
    char filename[FAT32_MAX_FILENAME_LEN];
    uint32_t size;
    struct tm* ctime;
}FAT32_Metadata;

typedef struct {
    FAT32_Metadata metadata;
    ClusterList clusterList;
    off_t fileEntryLocation;
    uint8_t attributes;
    bool isLast;
}FAT32_File;


FAT32Info getFAT32Info(int vol_fd);
uint8_t isFAT(int vol_fd);
void FAT32_init(int vol_fd);
FAT32_File* FAT32_SearchInVolume(int vol_fd, char* target);
FAT32_File* FAT32_SearchInRoot(int vol_fd, char* target);
void catFAT32File(int vol_fd, ClusterList cl, uint32_t size);
bool FAT32_SetCreationDate(int vol_fd, char* target, char* value);
bool FAT32_SetRonlyFlag(int vol_fd, char* target, bool value);
bool FAT32_SetHiddenFlag(int vol_fd, char* target, bool value);


#endif //SOA_RAGNAROK_FAT32_H