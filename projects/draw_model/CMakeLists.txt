cmake_minimum_required(VERSION 3.0)
project (draw_model)


# libraries
set (LIB_DIR ../../libraries)
set (LIB_BIN_DIR ../../libraries/bin)

set (GLFW_INCLUDE ${LIB_DIR}/glfw/include)
set (GLM_INCLUDE ${LIB_DIR}/glm)

set (GLFW_BIN_DIR ${LIB_BIN_DIR}/glfw/)
set (GLFW_BIN_VS2015 ${GLFW_BIN_DIR}/lib-vc2015)

set (STB_IMAGE_INCLUDE ${LIB_DIR}/stb)
set (TINY_OBJ_INC ${LIB_DIR}/tinyobjloader)

set (COMMON_DIR ../../common)

# source files
set (SRC_DIR src)
set (SRC_FILES ${SRC_DIR}/main.cpp 
               ${SRC_DIR}/HelloModel.cpp 
               ${SRC_DIR}/HelloModel.h)

# Find vulkan
find_path(VULKAN_INCLUDE_DIR NAMES vulkan/vulkan.h PATHS "$ENV{VULKAN_SDK}/Include")
find_library (VK_SDK NAMES vulkan-1 HINTS "$ENV{VK_SDK_PATH}/Lib32")

if (NOT VULKAN_INCLUDE_DIR)
	message (FATAL_ERROR "vulkan include dir not found")
endif()	

if (NOT VK_SDK)	
	message (FATAL_ERROR "Vulkan sdk not found")
endif()	

# Find Glfw static lib
find_library (GLFW_BIN NAMES glfw3 HINTS "${GLFW_BIN_VS2015}")

if (NOT GLFW_BIN)	
	message (FATAL_ERROR "Glfw binary not found")
endif()


# include everything in the executable
include_directories (${COMMON_DIR}
                     ${GLFW_INCLUDE}
                     ${GLM_INCLUDE}
                     ${VULKAN_INCLUDE_DIR}
                     ${STB_IMAGE_INCLUDE}
                     ${TINY_OBJ_INC}
                     )	

add_executable(draw_model ${SRC_FILES})

target_link_libraries (draw_model ${VK_SDK} ${GLFW_BIN})