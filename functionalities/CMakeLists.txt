cmake_minimum_required(VERSION 3.8)
project(functionalities)

set(
        FUNCT_SOURCE_FILES
        functionalities.c
        functionalities.h)

add_library(${PROJECT_NAME} ${FUNCT_SOURCE_FILES})