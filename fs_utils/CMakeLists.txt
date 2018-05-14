cmake_minimum_required(VERSION 3.8)
project(fs_utils)

set(
        FS_UTILS_SOURCE_FILES
        common.c common.h ext4.c ext4.h fat32.c fat32.h)

add_library(${PROJECT_NAME} ${FS_UTILS_SOURCE_FILES})