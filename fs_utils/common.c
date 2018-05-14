//
// Created by GDL on 7/5/18.
//
#include <fcntl.h>
#include "common.h"
#include "../lib/exitCodes.h"
#include <stdlib.h>
#include <stdio.h>
#include "../fs_utils/ext4.h"
#include "../fs_utils/fat32.h"



int initVolume(char* path) {
    int fd = open(path, O_RDONLY);
    if(fd == -1){
        fprintf(stderr, "Can't open file descriptor to provided volume at path '%s'\n", path);
        exit(CANNOT_OPEN_FD_TO_VOLUME);
    }
    return fd;
}


uint8_t detectSupportedFileSystem(int vol_fd) {
    bool ext = isExt(vol_fd);
    uint8_t fat = isFAT(vol_fd);

    if(ext){
        if(fat == NONE){
            uint8_t whichExt = getExtVersion(vol_fd);
            switch(whichExt){
                case EXT2:
                    fprintf(stdout, "File System not supported (EXT2)\n");
                    exit(FILE_SYSTEM_NOT_SUPPORTED);
                case EXT3:
                    fprintf(stdout, "File System not supported (EXT3)\n");
                    exit(FILE_SYSTEM_NOT_SUPPORTED);
                case EXT4:
                    return EXT4;
            }
        }
    } else if(fat != NONE){
        switch(fat){
            case FAT12:
                fprintf(stdout, "File System not supported (FAT12)\n");
                exit(FILE_SYSTEM_NOT_SUPPORTED);
            case FAT16:
                fprintf(stdout, "File System not supported (FAT16)\n");
                exit(FILE_SYSTEM_NOT_SUPPORTED);
            case FAT32:
                return FAT32;
        }
    }

    fprintf(stderr, "Can't assert filesystem type. Directors are present for neither or both EXT and FAT filesystems\n");
    exit(CANNOT_ASSERT_FILESYSTEM);
}