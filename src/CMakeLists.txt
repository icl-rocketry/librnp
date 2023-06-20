cmake_minimum_required(VERSION 3.16.3)

add_library(librnp STATIC)

#collect source files
file(GLOB_RECURSE LIBRNP_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
target_sources(librnp PUBLIC ${LIBRNP_SRCS})

#get include dcirectories relative to the current directory, i.e we have the prefix librnp/...
target_include_directories(librnp PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})


