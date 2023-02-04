# Set minimum CMake version
cmake_minimum_required(VERSION 3.16.3)

# Declare project
project(rnp LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)

# Set output directories
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)

# Add source directory
add_subdirectory(src)

# Add tests
add_subdirectory(tests)