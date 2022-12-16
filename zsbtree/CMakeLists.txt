cmake_minimum_required(VERSION 3.16)
project(zsbtree)


include_directories(../sax/include)
include_directories(../util)
include_directories(../include/leveldb)

file(GLOB_RECURSE ZSBTREE_LIB_SRC
        ./*.cc
        ./*.h
        ../util/newVector.h
        ../util/newVector.cc
        ../include/leveldb/table.h
        )
add_library(zsbtree STATIC ${ZSBTREE_LIB_SRC})

