//
// Created by GDL on 11/4/18.
//

#ifndef SOA_RAGNAROK_FS_DETECTOR_H
#define SOA_RAGNAROK_FS_DETECTOR_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "../lib/exitCodes.h"
#include "../functionalities/fimbulvetr.h"

#define VALID 0x01
#define INVALID 0x00

#define COMMAND_INFO    "-info"
#define COMMAND_SEARCH  "-search"


void assertArgs(int, char**);
void executeCommand(char**);

#endif //SOA_RAGNAROK_FS_DETECTOR_H
