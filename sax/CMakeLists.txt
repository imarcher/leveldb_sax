cmake_minimum_required(VERSION 3.16)
project(sax)

include_directories(include)


file(GLOB_RECURSE SAX_LIB_SRC
        src/*.cc
        )
add_library(sax STATIC ${SAX_LIB_SRC})


#add_executable(main main.cc)
#target_link_libraries(main sax)
