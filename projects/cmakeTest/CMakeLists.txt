cmake_minimum_required (VERSION 2.6)

project (cmake_test)

set (SRC_DIR src)
set (SRC_FILES main.cpp)

add_executable (cmake_test ${SRC_DIR}/${SRC_FILES})

