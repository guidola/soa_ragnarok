cmake_minimum_required(VERSION 3.8)
project(fs_utils)

set(
        FS_UTILS_SOURCE_FILES
        fs_detector.c
        fs_detector.h)

add_library(${PROJECT_NAME} ${FS_UTILS_SOURCE_FILES})