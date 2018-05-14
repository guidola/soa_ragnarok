//
// Created by GDL on 7/5/18.
//

#ifndef SOA_RAGNAROK_COMMON_H
#define SOA_RAGNAROK_COMMON_H

#include <stdint.h>

int initVolume(char* path);
uint8_t detectSupportedFileSystem(int vol_fd);

#endif //SOA_RAGNAROK_COMMON_H
