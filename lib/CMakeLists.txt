cmake_minimum_required(VERSION 3.8)
project(lib)

set(
        LIBRARY_SOURCE_FILES
        hashtable.c
        hashtable.h
        processTree.c
        processTree.h
        tools.c
        tools.h
        semaphore.c
        semaphore.h
        orderedBuffer.c
        orderedBuffer.h
        roundRobin.c
        roundRobin.h exitCodes.h stringUtils.c stringUtils.h types.h)

add_library(${PROJECT_NAME} ${LIBRARY_SOURCE_FILES})