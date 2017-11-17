cmake_minimum_required(2.6)
project (test_project_1)

set (SRC_DIR src)
set (SRC_FILES main.cpp)
set (LIB_DIR ../../libraries)
set (LIB_BIN_DIR ../../libraries/bin)
set (GLFW_BIN_DIR ${LIB_BIN_DIR}/glfw/)
set (GLFW_BIN_VS2015 ${GLFW_BIN_DIR}/vs2015)

find_library (VK_SDK
	NAMES VulkanSDK vulkan
	PATHS $ENV{VK_SDK_PATH} $ENV{VULKAN_SDK}
	)

find_library (GFLW_BIN
	NAMES glfw
	PATHS ${GLFW_BIN_VS2015}
add_executable (test_project_1 ${SRC_DIR}/${SRC_FILES})

if (VK_SDK)
	target_link_libraries(test_project_1 ${VK_SDK})
endif()