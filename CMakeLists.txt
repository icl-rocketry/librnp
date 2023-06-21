# Set minimum CMake version
cmake_minimum_required(VERSION 3.16.3)

if (ESP_PLATFORM)
    idf_component_register(INCLUDE_DIRS src
                           REQUIRES arduino app_update) #requires arduino....
    #we need some wizardry so we keep the nice original cmake structure
    #we add the src subdirectory where the static library is defined
    add_subdirectory(src) 
    #we need to explictly link the arduino component here as librnp has a couple dependancies on the arduino library when used for esp32
    target_link_libraries(librnp PRIVATE idf::arduino) 
    target_link_libraries(librnp PRIVATE idf::app_update) 
    #link the orignal cmake librnp library to the component library 
    target_link_libraries(${COMPONENT_LIB} INTERFACE librnp)
    return()

endif()


# Declare project
project(librnp LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)

# Set output directories
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)

# Add source directory
add_subdirectory(src)

# Add tests
option(TESTS "enable building of tests" OFF)
if (TESTS)
    add_subdirectory(tests)
endif()