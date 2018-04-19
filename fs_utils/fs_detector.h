//
// Created by GDL on 11/4/18.
//

#ifndef SOA_RAGNAROK_FS_DETECTOR_H
#define SOA_RAGNAROK_FS_DETECTOR_H

#include "../lib/types.h"

#define FAT32   0x01
#define FAT64   0x02
#define EXT2    0x03
#define EXT3    0x04
#define EXT4    0x05

typedef struct uint8_t FileSystem;

FileSystem detectFilesystem(FileDescriptor fd);

#endif //SOA_RAGNAROK_FS_DETECTOR_H
