cmake_minimum_required(VERSION 3.16)
project(zsbtree)


include_directories(../sax/include)
include_directories(../util)


file(GLOB_RECURSE ZSBTREE_LIB_SRC
        ./*.cc
        ./*.h
        ../util/arena.cc
        ../util/arena.h
        )
add_library(zsbtree STATIC ${ZSBTREE_LIB_SRC})

