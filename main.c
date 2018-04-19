#include "shell/commands.h"

int main(int argc, char** argv) {

    assertArgs(argc, argv);
    executeCommand(argv);

    return 0;
}