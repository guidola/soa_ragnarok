//
// Created by GDL on 7/5/18.
//

#include <strings.h>
#include <stdlib.h>
#include <time.h>
#include "fat32.h"
#include "../lib/exitCodes.h"

typedef struct {
    uint32_t rootDirStartCluster;
    uint32_t clusterSize;
    uint32_t sectorsPerFat;
    uint8_t fatCount;
    uint64_t fatsSize;
    uint16_t sectorSize;
    uint8_t sectorsPerCluster;
    uint16_t reservedSectorsCount;
    uint64_t fatAreaOffset;
    uint64_t dataAreaOffset;
}EXT4_Globals;

EXT4_Globals globals;

void seekToBootSectorField(int vol_fd, off_t offset) {
    lseek(vol_fd, BOOT_SECTOR_START + offset, SEEK_SET);
}

void readBootSectorField(int vol_fd, void* destination, size_t size, off_t offset) {
    seekToBootSectorField(vol_fd, offset);
    read(vol_fd, destination, size);
}

void seekToFatEntry(int vol_fd, uint64_t cluster_number) {
    lseek(vol_fd, globals.fatAreaOffset + cluster_number * FAT32_ENTRY_SIZE, SEEK_SET);
}

uint32_t readFatEntry(int vol_fd, uint64_t cluster_number) {
    seekToFatEntry(vol_fd, cluster_number);
    uint32_t destination;
    read(vol_fd, &destination, sizeof(destination));
    return destination & FAT32_ENTRY_MASK;
}

ClusterList getClusterList(int vol_fd, uint32_t firstCluster) {

    ClusterList cl;
    cl.count = 0;
    if(firstCluster > EOC_FLAG_THRESHOLD) return cl;
    cl.clusters = (uint32_t*)calloc(CLUSTER_LIST_REALLOC_SIZE, sizeof(uint32_t));
    cl.size = CLUSTER_LIST_REALLOC_SIZE;
    uint32_t nextCluster = firstCluster;
    cl.clusters[cl.count++] = firstCluster;

    while(true){

        nextCluster = readFatEntry(vol_fd, nextCluster);
        if(nextCluster > EOC_FLAG_THRESHOLD){
            return cl;
        }

        cl.clusters[cl.count++] = nextCluster;
        if(cl.count >= cl.size){
            cl.clusters = (uint32_t*)realloc(cl.clusters, cl.size + CLUSTER_LIST_REALLOC_SIZE * sizeof(uint32_t));
        }

    }

}

void readDirectoryEntryField(int vol_fd, void* destination, size_t size, off_t entry_base, off_t offset){
    lseek(vol_fd, entry_base + offset, SEEK_SET);
    read(vol_fd, destination, size);
}

bool filenameHasBeenShortened(const char filename[FAT32_MAX_FILENAME_LEN]){

    for (int i = 0; i < SHORT_FILE_NAME; i++) {
        if(filename[i] == '~'){
            return true;
        }
    }

    return false;
}

bool isVFatEntry(int vol_fd, off_t initial_position){

    uint8_t attrs;
    readDirectoryEntryField(vol_fd, &(attrs), sizeof(uint8_t), initial_position, FILE_ATTRS);
    if(attrs == VFAT_LONGNAME_ENTRY_FLAG){
        return true;
    }

    return false;
}

off_t processLongFileName(int vol_fd, off_t initial_position, char* filename, uint16_t actualPos){

    char str_aux[FAT32_MAX_FILENAME_LEN];
    readDirectoryEntryField(vol_fd, str_aux, VFAT_NAME_SLOT1_SIZE, initial_position, VFAT_NAME_SLOT1);
    readDirectoryEntryField(vol_fd, &(str_aux[VFAT_NAME_SLOT1_SIZE]), VFAT_NAME_SLOT2_SIZE, initial_position, VFAT_NAME_SLOT2);
    readDirectoryEntryField(vol_fd, &(str_aux[VFAT_NAME_SLOT1_SIZE + VFAT_NAME_SLOT2_SIZE]), VFAT_NAME_SLOT3_SIZE, initial_position, VFAT_NAME_SLOT3);

    //use only the lower byte of the USC to convert it to ascii
    int i = 0;
    for (i = 0; i < NAME_BYTES_PER_VFAT_ENTRY/2; i++) {
        if(str_aux[2 * i] == '\0') {
            break;
        }
        str_aux[i] = str_aux[2 * i];
    }
    str_aux[i] = '\0';
    strcat(str_aux, filename);
    actualPos += i;
    strcpy(filename, str_aux);
    
    uint8_t counter;
    readDirectoryEntryField(vol_fd, &counter, sizeof(counter), initial_position, VFAT_SN);

    if((counter & VFAT_SN_VAL_MASK) == VFAT_SN_FIRST){
        return initial_position + DIR_ENTRY_SIZE;
    } else {
        return processLongFileName(vol_fd, initial_position + DIR_ENTRY_SIZE, filename, actualPos);
    }

}

//assumes that the fd is positioned at the beginning of a directory entry
FAT32_File* getFAT32File(int vol_fd) {

    off_t initial_position = lseek(vol_fd, 0, SEEK_CUR);
    FAT32_File* f = (FAT32_File*)calloc(1, sizeof(FAT32_File));
    char filename[SHORT_FILE_NAME_SIZE+1];

    readDirectoryEntryField(vol_fd, &(filename), SHORT_FILE_NAME_SIZE * sizeof(char), initial_position, SHORT_FILE_NAME);

    if((uint8_t)(filename[0]) == ERASED_FILE_FLAG_VALUE) {
        lseek(vol_fd, initial_position + DIR_ENTRY_SIZE, SEEK_SET);
        return NULL;

    } else if((uint8_t)(filename[0]) == LAST_ENTRY_FLAG_VALUE){
        f->isLast = true;
        return f;
    } else if(isVFatEntry(vol_fd, initial_position)){
        initial_position = processLongFileName(vol_fd, initial_position, f->metadata.filename, 0);
    } else {
        for (int i = SHORT_FILE_NAME_SIZE-1; i >= 0; i--) {
            if(filename[i] != ' '){
                filename[i+1] = '\0';
                break;
            }
        }
        strcpy(f->metadata.filename, filename);
        char ext[SHORT_FILE_EXT_LEN+1];
        readDirectoryEntryField(vol_fd, &(ext), SHORT_FILE_EXT_LEN * sizeof(char), initial_position, SHORT_FILE_EXT);
        for (int i = SHORT_FILE_EXT_LEN-1; i > 0; i--) {
            if(ext[i] != ' '){
                ext[i+1] = '\0';
                break;
            }
        }
        if(ext[0] != ' ' && ext[0] != '\0') {
            strcat(f->metadata.filename, ".\0");
            strcat(f->metadata.filename, ext);
        }

    }

    if(strcmp(f->metadata.filename, ".\0") == 0 || strcmp(f->metadata.filename, "..\0") == 0){
        lseek(vol_fd, initial_position + DIR_ENTRY_SIZE, SEEK_SET);
        return NULL;
    }

    uint16_t date_aux;
    uint8_t sec_aux;

    readDirectoryEntryField(vol_fd, &(sec_aux), sizeof(sec_aux), initial_position, CREATION_TIME_FINE);
    readDirectoryEntryField(vol_fd, &(date_aux), sizeof(date_aux), initial_position, CREATION_TIME_BASE);
    f->metadata.ctime = (struct tm*)calloc(1, sizeof(struct tm));
    f->metadata.ctime->tm_min = (date_aux & CTIME_BASE_MASK_MINUTES) >> MINUTES_OFFSET;
    f->metadata.ctime->tm_sec = (date_aux & CTIME_BASE_MASK_SECONDS) + (uint16_t)floor((sec_aux * 10)/1000);
    f->metadata.ctime->tm_hour = (date_aux & CTIME_BASE_MASK_HOURS) >> HOURS_OFFSET;
    readDirectoryEntryField(vol_fd, &(date_aux), sizeof(date_aux), initial_position, CDATE_BASE);
    f->metadata.ctime->tm_mday = date_aux & CDATE_BASE_MASK_DAY;
    f->metadata.ctime->tm_mon = (date_aux & CDATE_BASE_MASK_MONTH) >> MONTH_OFFSET;
    f->metadata.ctime->tm_year = BASE_YEAR + ((date_aux & CDATE_BASE_MASK_YEAR) >> YEAR_OFFSET);

    readDirectoryEntryField(vol_fd, &(f->metadata.size), sizeof(f->metadata.size), initial_position, FILE_SIZE);
    readDirectoryEntryField(vol_fd, &(f->attributes), sizeof(f->attributes), initial_position, FILE_ATTRS);

    uint32_t firstCluster = 0;
    readDirectoryEntryField(vol_fd, &firstCluster, sizeof(uint16_t), initial_position, FIRST_CLUSTER_HI);
    firstCluster = firstCluster << UINT32_NIBBLE_BITS;
    readDirectoryEntryField(vol_fd, &firstCluster, sizeof(uint16_t), initial_position, FIRST_CLUSTER_LO);
    f->clusterList = getClusterList(vol_fd, firstCluster);
    f->fileEntryLocation = initial_position;

    lseek(vol_fd, initial_position + DIR_ENTRY_SIZE, SEEK_SET);
    return f;
}

void seekToCluster(int vol_fd, uint32_t cluster) {
    lseek(vol_fd, globals.dataAreaOffset + (cluster - 2) * globals.clusterSize, SEEK_SET);
}

bool isFileVisible(FAT32_File f, bool isSearch){
    return (f.attributes & HIDDEN_ATTR_MASK) == 0 || !isSearch;
}

FAT32_File* FAT32_searchRecursivelyInDirectory(int vol_fd, char* target, uint32_t initialCluster, bool isSearch){

    ClusterList cl = getClusterList(vol_fd, initialCluster);
    for(uint64_t i = 0; i < cl.count; i++){

        seekToCluster(vol_fd, cl.clusters[i]);

        for (uint32_t j = 0; j < globals.clusterSize; j+= DIR_ENTRY_SIZE) {
            FAT32_File* f = getFAT32File(vol_fd);

            if(f == NULL) continue;

            if(f->isLast == true) break;

            if((f->attributes & SUBDIRECTORY_ATTR_MASK) == SUBDIRECTORY_ATTR_MASK){
                off_t currentPosition = lseek(vol_fd, 0, SEEK_CUR);
                f = FAT32_searchRecursivelyInDirectory(vol_fd, target, f->clusterList.clusters[0], isSearch);
                if(f != NULL) return f;
                lseek(vol_fd, currentPosition, SEEK_SET);

            } else if((f->attributes & VOLUME_LABEL_ATTR_MASK) == 0 && isFileVisible(*f, isSearch) &&
                    strcmp(target, f->metadata.filename) == 0) {

                return f;
            }
        }

    }

    return NULL;
}

FAT32_File* FAT32_searchInDirectory(int vol_fd, char* target, uint32_t initialCluster){

    ClusterList cl = getClusterList(vol_fd, initialCluster);
    for(uint64_t i = 0; i < cl.count; i++){

        seekToCluster(vol_fd, cl.clusters[i]);

        for (uint32_t j = 0; j < globals.clusterSize; j+= DIR_ENTRY_SIZE) {
            FAT32_File* f = getFAT32File(vol_fd);

            if(f == NULL) continue;

            if(f->isLast == true) break;

            if((f->attributes & (SUBDIRECTORY_ATTR_MASK|VOLUME_LABEL_ATTR_MASK)) == 0 && isFileVisible(*f, true) &&
                    strcmp(target, f->metadata.filename) == 0){

                return f;
            }

        }

    }

    return NULL;
}

void catFAT32File(int vol_fd, ClusterList cl, uint32_t size) {

    Cluster cluster = (Cluster)calloc(globals.clusterSize, sizeof(uint8_t));

    for(uint64_t i = 0; i < cl.count; i++){

        seekToCluster(vol_fd, cl.clusters[i]);
        read(vol_fd, cluster, sizeof(uint8_t) * globals.clusterSize);

        if(cluster[0] == '\0') continue;

        if(i == cl.count - 1){
            write(1, cluster, (size_t) (sizeof(uint8_t) * (size % globals.clusterSize)));
        } else {
            write(1, cluster, (size_t) (sizeof(uint8_t) * globals.clusterSize));
        }

    }

    write(1, "\n\n", 2);
    free(cluster);

}

FAT32_File* FAT32_SearchInVolume(int vol_fd, char* target){
    return FAT32_searchRecursivelyInDirectory(vol_fd, target, globals.rootDirStartCluster, true);
}

FAT32_File* FAT32_SearchInRoot(int vol_fd, char* target){
    return FAT32_searchInDirectory(vol_fd, target, globals.rootDirStartCluster);
}


void FAT32_init(int vol_fd){

    readBootSectorField(vol_fd, &(globals.reservedSectorsCount), sizeof(globals.reservedSectorsCount), RESERVED_SECTORS_COUNT);
    readBootSectorField(vol_fd, &(globals.sectorsPerFat), sizeof(globals.sectorsPerFat), SECTORS_PER_FAT);
    readBootSectorField(vol_fd, &(globals.fatCount), sizeof(globals.fatCount), FAT_COUNT);
    readBootSectorField(vol_fd, &(globals.sectorsPerCluster), sizeof(globals.sectorsPerCluster), SECTORS_PER_CLUSTER);
    readBootSectorField(vol_fd, &(globals.sectorSize), sizeof(globals.sectorSize), BYTES_PER_LOGIC_SECTOR);
    readBootSectorField(vol_fd, &(globals.rootDirStartCluster), sizeof(globals.rootDirStartCluster), ROOT_DIR_START_CLUSTER);
    globals.clusterSize = globals.sectorsPerCluster * globals.sectorSize;
    globals.fatAreaOffset = globals.sectorSize * globals.reservedSectorsCount;
    globals.dataAreaOffset = globals.fatAreaOffset + globals.fatCount * globals.sectorsPerFat * globals.sectorSize;

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
        info.label[11] = '\0';
    } else {
        strcpy(info.label, "-\0");
    }
    readBootSectorField(vol_fd, &(info.maximumRootEntries), sizeof(info.maximumRootEntries), MAX_ROOT_DIR_ENTRIES);
    readBootSectorField(vol_fd, &(info.sectorsPerFat), sizeof(info.sectorsPerFat), SECTORS_PER_FAT);
    readBootSectorField(vol_fd, &(info.systemName), sizeof(char) * 8, SYSTEM_NAME);

    return info;
}

void writeToDirectoryEntry(int vol_fd, off_t entryPos, void* value, size_t size, off_t offset) {
    lseek(vol_fd, entryPos + offset, SEEK_SET);
    write(vol_fd, value, size);
}

bool FAT32_SetHiddenFlag(int vol_fd, char* target, bool value) {

    FAT32_File* f = FAT32_searchRecursivelyInDirectory(vol_fd, target, globals.rootDirStartCluster, false);
    if(f == NULL){
        return false;
    }

    uint8_t attrs = (uint8_t)((f->attributes & HIDDEN_ATTR_CLEAR) | ((value ? FLAG_UP : FLAG_DOWN) << HIDDEN_ATTR_OFFSET));

    writeToDirectoryEntry(vol_fd, f->fileEntryLocation, &attrs, sizeof(attrs), FILE_ATTRS);
    readDirectoryEntryField(vol_fd, &attrs, 1, f->fileEntryLocation, FILE_ATTRS);

    return true;

}

bool FAT32_SetRonlyFlag(int vol_fd, char* target, bool value) {

    FAT32_File* f = FAT32_searchRecursivelyInDirectory(vol_fd, target, globals.rootDirStartCluster, false);
    if(f == NULL){
        return false;
    }

    uint8_t attrs = (f->attributes & RONLY_ATTR_CLEAR) | (value ? FLAG_UP : FLAG_DOWN);

    writeToDirectoryEntry(vol_fd, f->fileEntryLocation, &attrs, sizeof(attrs), FILE_ATTRS);
    attrs = 0;
    readDirectoryEntryField(vol_fd, &attrs, 1, f->fileEntryLocation, FILE_ATTRS);
    return true;

}

bool FAT32_SetCreationDate(int vol_fd, char* target, char* value) {

    FAT32_File* f = FAT32_searchRecursivelyInDirectory(vol_fd, target, globals.rootDirStartCluster, false);
    if(f == NULL){
        return false;
    }

    struct tm tm_crDate;
    char* ret = strptime(value, "%d%m%Y", &tm_crDate);

    if(ret == NULL || *ret != '\0') exit(INVALID_DATE_FORMAT);

    uint32_t date = (uint32_t)tm_crDate.tm_mday + ((uint32_t)tm_crDate.tm_mon << MONTH_OFFSET) +
            ((uint32_t)(tm_crDate.tm_year - BASE_YEAR) << YEAR_OFFSET);

    writeToDirectoryEntry(vol_fd, f->fileEntryLocation, &date, sizeof(date), CDATE_BASE);
    return true;

}
