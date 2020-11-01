cmake_minimum_required(VERSION 2.8)

SET(CMAKE_C_COMPILER "/usr/bin/gcc")
SET(CMAKE_CXX_COMPILER "/usr/bin/g++")

#aux_source_directory(${CMAKE_SOURCE_DIR}/backend backend)
#set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}) #I put cpplint.cmake in $CMAKE_SOURCE_DIR
#include(cpplint)
#add_style_check_target(phoenix-cpplint "${src}") 
add_custom_target(lint COMMAND ${CMAKE_COMMAND} -P ${CMAKE_SOURCE_DIR}/backend/cpplint.cmake)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2 -fPIC")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -std=c++11 -Wall -ggdb -fPIC")

project(hft)
add_subdirectory(src cmake_src)
execute_process(COMMAND ${PYTHON} cpplint.py)
