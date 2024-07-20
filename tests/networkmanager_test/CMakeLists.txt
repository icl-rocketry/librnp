

cmake_minimum_required(VERSION 3.16.0)

project(networkmanager_test)

add_compile_options(-g)
add_compile_options(-O0)
add_compile_options(-Wall)
add_compile_options(-Wpedantic)




# add_executable(libriccore_fsm_test ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp ${LIBRNP_SRC})
add_executable(networkmanager_test ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)

target_compile_features(networkmanager_test PRIVATE cxx_std_17)
target_include_directories(networkmanager_test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(networkmanager_test librnp)

