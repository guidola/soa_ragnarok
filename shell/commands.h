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
#include "../functionalities/functionalities.h"

#define VALID 0x01
#define INVALID 0x00

#define COMMAND_INFO    "-info"
#define COMMAND_SEARCH  "-search"
#define COMMAND_DEEP_SEARCH "-deepsearch"
#define COMMAND_SHOW "-show"
#define COMMAND_RONLY "-r"
#define COMMAND_WR "-w"
#define COMMAND_HIDE "-h"
#define COMMAND_UNHIDE "-s"
#define COMMAND_CHANGE_DATE "-d"


void assertArgs(int, char**);
void executeCommand(char**);

#endif //SOA_RAGNAROK_FS_DETECTOR_H
