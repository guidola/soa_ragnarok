//
// Created by GDL on 11/4/18.
//

#include "commands.h"

uint8_t __isValidCommand(char* cmd, int argc) {
    if(strcmp(cmd, COMMAND_INFO) == 0){
        return VALID;

    } else if(strcmp(cmd, COMMAND_SEARCH) == 0 ||
            strcmp(cmd, COMMAND_DEEP_SEARCH) == 0 ||
            strcmp(cmd, COMMAND_RONLY) == 0 ||
            strcmp(cmd, COMMAND_WR) == 0 ||
            strcmp(cmd, COMMAND_HIDE) == 0 ||
            strcmp(cmd, COMMAND_UNHIDE) == 0 ||
            strcmp(cmd, COMMAND_CHANGE_DATE) == 0 ||
            strcmp(cmd, COMMAND_SHOW) == 0){
        if(argc < 4){
            fprintf(stderr, "Missing target. USAGE: %s {-search|-deepsearch|-show|-r|-w|-h|-s|-d} <volume> <target>", cmd);
            return INVALID;
        }
        return VALID;
    }

    fprintf(stderr, "Invalid command. USAGE: %s {-info|-search} <volume> [<target>]\n", cmd);
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
        fprintf(stderr, "Invalid number of parameters. USAGE: %s {-info|-search|-deepsearch|-show|-r|-w|-h|-s|-d} <volume> [<target>]\n", argv[0]);
        exit(FAILED_TO_ASSERT_PARAMS);

    } else if(!__isValidCommand(argv[1], argc)) {
        exit(INVALID_COMMAND);
    }

    if(strcmp(argv[1], COMMAND_CHANGE_DATE) == 0 ){
        __checkForVolumeAvailability(argv[3]);
    } else {
        __checkForVolumeAvailability(argv[2]);
    }


}

void executeCommand(char** parameters) {

    if(strcmp(parameters[1], COMMAND_INFO) == 0) {
        showFileSystemInfo(parameters[2]);

    } else if(strcmp(parameters[1], COMMAND_SEARCH) == 0) {
        searchForFileInVolume(parameters[2], parameters[3], false);

    } else if(strcmp(parameters[1], COMMAND_DEEP_SEARCH) == 0) {
        searchForFileInVolume(parameters[2], parameters[3], true);

    } else if(strcmp(parameters[1], COMMAND_SHOW) == 0) {
        searchAndShowFileInVolume(parameters[2], parameters[3]);

    } else if(strcmp(parameters[1], COMMAND_RONLY) == 0){
        setFileWriteOnly(parameters[2], parameters[3], true);

    } else if(strcmp(parameters[1], COMMAND_WR) == 0){
        setFileWriteOnly(parameters[2], parameters[3], false);

    } else if(strcmp(parameters[1], COMMAND_HIDE) == 0){
        setFileHidden(parameters[2], parameters[3], true);

    } else if(strcmp(parameters[1], COMMAND_UNHIDE) == 0){
        setFileHidden(parameters[2], parameters[3], false);

    } else if(strcmp(parameters[1], COMMAND_CHANGE_DATE) == 0){
        setFileCreationDate(parameters[3], parameters[4], parameters[2]);
    }

    fprintf(stdout, "\n");

}



