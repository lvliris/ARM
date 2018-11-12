cmake_minimum_required(VERSION 2.8)
#set the compiler
#SET(CMAKE_C_COMPILER ${TOOLCHAIN_PREFIX}gcc)
#SET(CMAKE_CXX_COMPILER ${TOOLCHAIN_PREFIX}g++)
#SET(CMAKE_LINKER ${TOOLCHAIN_PREFIX}ld)
#SET(CMAKE_C_LINK_EXECUTABLE ${TOOLCHAIN_PREFIX}ld)
#set (CMAKE_CXX_STANDARD 11)
#project name
PROJECT(master)
#head file path
INCLUDE_DIRECTORIES(
include
)
#source directory
AUX_SOURCE_DIRECTORY(src DIR_SRCS)
message(status "${DIR_SRCS}")
#set environment variable
SET(MASTER
${DIR_SRCS}
)
#set extern libraries
LINK_DIRECTORIES(./lib)
message(status "${DIR_SRCS}")
#add executable file
ADD_EXECUTABLE(master ${MASTER})
#add link lib
FIND_PACKAGE(Threads)
TARGET_LINK_LIBRARIES(master ${CMAKE_THREAD_LIBS_INIT})
TARGET_LINK_LIBRARIES(master curl)
