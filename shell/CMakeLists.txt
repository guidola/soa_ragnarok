cmake_minimum_required(VERSION 3.8)
project(shell)

set(
        SHELL_SOURCE_FILES
        commands.c
        commands.h)

add_library(${PROJECT_NAME} ${SHELL_SOURCE_FILES})