//
// Created by GDL on 19/4/18.
//

#ifndef SOA_RAGNAROK_FIMBULVETR_H
#define SOA_RAGNAROK_FIMBULVETR_H

#include "../fs_utils/common.h"
#include "../fs_utils/fat32.h"
#include "../fs_utils/ext4.h"

#include <stdbool.h>
#include <time.h>
#include <stdlib.h>


void showFileSystemInfo(char* volume);
void searchForFileInVolume(char* volume, char* target, bool isRecursive);
void searchAndShowFileInVolume(char* volume, char* target);
void setFileWriteOnly(char* volume, char* target, bool value);
void setFileHidden(char* volume, char* target, bool value);
void setFileCreationDate(char* volume, char* target, char* value);

#endif //SOA_RAGNAROK_FIMBULVETR_H
