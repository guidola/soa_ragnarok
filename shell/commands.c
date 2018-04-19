//
// Created by GDL on 11/4/18.
//

#include "commands.h"

uint8_t __isValidCommand(char* cmd) {
    if(strcmp(cmd, "-info") == 0){
        return VALID;

    }// add as much else ifs as commands we got

    return INVALID;
}

void __checkForVolumeAvailability(char* volume) {


    if( access( volume, F_OK ) == -1 ) {
        fprintf(stderr, "Filesystem not found. Can't stat provided volume %s.\n", volume);
        exit(VOLUME_DOES_NOT_EXIST);

    } else if( access( volume, R_OK ) == -1 ) {
        fprintf(stderr, "Current user does not have read permissions to the provided volume '%s'.\n", volume);
        exit(NOT_ENOUGH_PRIVILEGES);
    }

}

void assertArgs(int argc, char** argv) {

    if(argc < 3){
        fprintf(stderr, "Invalid number of parameters. USAGE: %s [-info] <volume>\n", argv[0]);
        exit(FAILED_TO_ASSERT_PARAMS);

    } else if(!__isValidCommand(argv[1])) {
        fprintf(stderr, "Invalid command. USAGE: %s [-info] <volume>\n", argv[0]);
        exit(INVALID_COMMAND);

    }

    __checkForVolumeAvailability(argv[2]);

}

void executeCommand(char** parameters) {

    if(strcmp(parameters[1], "-info") == 0) {
        showFileSystemInfo(parameters[2]);

    }// add as much else ifs as rutines we can execute

}



