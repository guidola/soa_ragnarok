cmake_minimum_required(VERSION 3.8)
project(soa_ragnarok)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -m64")
#set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.c)
add_executable(soa_ragnarok ${SOURCE_FILES})

set(FIMBULVETR_SOURCE_FILES
        fimbulvetr/fs_detector.c
        fimbulvetr/fs_detector.h)

add_library(fimbulvetr ${FIMBULVETR_SOURCE_FILES})
add_subdirectory(lib)