cmake_minimum_required(VERSION 2.8)
#set the compiler
SET(CMAKE_c_COMPILER "/usr/local/arm/arm-2009q3/bin/arm-none-linux-gnueabi-gcc")
SET(CMAKE_CXX_COMPILER "/usr/local/arm/arm-2009q3/bin/arm-none-linux-gnueabi-g++")
#set (CMAKE_CXX_STANDARD 11)
#project name
PROJECT(master)
#head file path
INCLUDE_DIRECTORIES(
include
)
#source directory
AUX_SOURCE_DIRECTORY(src DIR_SRCS)
#set environment variable
SET(MASTER
${DIR_SRCS}
)
#set extern libraries

#add executable file
ADD_EXECUTABLE(./master ${MASTER})
#add link lib
FIND_PACKAGE(Threads)
TARGET_LINK_LIBRARIES(./master ${CMAKE_THREAD_LIBS_INIT})
