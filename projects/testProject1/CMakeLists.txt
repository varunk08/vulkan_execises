cmake_minimum_required( VERSION 2.6)
project (test_project_1)

set (SRC_DIR src)
set (SRC_FILES main.cpp)
set (LIB_DIR ../../libraries)
set (LIB_BIN_DIR ../../libraries/bin)

set (GLFW_INCLUDE ${LIB_DIR}/glfw/include)
set (GLM_INCLUDE ${LIB_DIR}/glm)


set (GLFW_BIN_DIR ${LIB_BIN_DIR}/glfw/)
set (GLFW_BIN_VS2015 ${GLFW_BIN_DIR}/lib-vc2015)

find_path(VULKAN_INCLUDE_DIR
    NAMES vulkan/vulkan.h
    PATHS
      "$ENV{VULKAN_SDK}/Include"
    )

if (NOT VULKAN_INCLUDE_DIR)
	message(FATAL_ERROR "vulkan include dir not found")
endif()	
	
find_library (VK_SDK vulkan-1)
	#NAMES VulkanSDK vulkan
	#PATHS $ENV{VK_SDK_PATH} $ENV{VULKAN_SDK}
	#)

if (NOT VK_SDK)	
	message (FATAL_ERROR "Vulkan sdk not found")
endif()	

find_library (GLFW_BIN
			  NAMES glfw3
			  PATHS ${GLFW_BIN_VS2015})

if (NOT GLFW_BIN)	
	message (FATAL_ERROR "Glfw binary not found")
endif()

include_directories (${GLFW_INCLUDE} ${GLM_INCLUDE} ${VULKAN_INCLUDE_DIR})	
add_executable (test_project_1 ${SRC_DIR}/${SRC_FILES})

target_link_libraries (test_project_1 ${VK_SDK} ${GLFW_BIN})