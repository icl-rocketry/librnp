# Collect source files
file(GLOB_RECURSE rnp_src ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

# Build library
add_library(rnp STATIC ${rnp_src})
target_include_directories(rnp PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})