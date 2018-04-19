cmake_minimum_required(VERSION 3.8)
project(soa_ragnarok)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -m64")
#set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.c)
add_subdirectory(fs_utils)
add_subdirectory(shell)
add_subdirectory(lib)
add_subdirectory(functionalities)


add_executable(soa_ragnarok ${SOURCE_FILES})
target_link_libraries(soa_ragnarok shell lib fs_utils functionalities)