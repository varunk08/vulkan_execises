cmake_minimum_required (VERSION 2.6)

project (cmake_test)

set (SRC_DIR src)
set (SRC_FILES main.cpp)

#test code
set (items_to_buy apple banana orange)

foreach (item ${items_to_buy})
	message ("Dont forget to buy one ${item}")
endforeach()	

#cache variable exampls
set (COMPILER_OPTION "visual studio" CACHE STRING "select a compiler")
set_property(CACHE COMPILER_OPTION PROPERTY STRINGS "visual studio" "clang" "gcc")


add_executable (cmake_test ${SRC_DIR}/${SRC_FILES})

