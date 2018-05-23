//
// Created by GDL on 7/5/18.
//

#ifndef SOA_RAGNAROK_COMMON_H
#define SOA_RAGNAROK_COMMON_H

#include "../lib/exitCodes.h"
#include "../fs_utils/ext4.h"
#include "../fs_utils/fat32.h"
#include <fcntl.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>


int initVolume(char* path);
uint8_t detectSupportedFileSystem(int vol_fd);

#endif //SOA_RAGNAROK_COMMON_H
