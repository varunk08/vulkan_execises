#pragma once

#define GLFW_INCLUDE_VULKAN

#include <GLFW\glfw3.h>
#include <glm\glm.hpp>

#include <algorithm>
#include <array>
#include <functional>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <set>
#include <vector>

#include "common.h"

constexpr int WIDTH  = 800;
constexpr int HEIGHT = 600;

const std::vector<const char*> validationLayers =
{
    "VK_LAYER_LUNARG_standard_validation"
};

const std::vector<const char*> deviceExtensions =
{
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

VkResult CreateDebugReportCallbackEXT(
    VkInstance                                instance,
    const VkDebugReportCallbackCreateInfoEXT* pCreateInfo,
    const VkAllocationCallbacks*              pAllocator,
    VkDebugReportCallbackEXT*                 pCallback);

void DestroyDebugReportCallbackEXT(
    VkInstance                   instance,
    VkDebugReportCallbackEXT     callback,
    const VkAllocationCallbacks* pAllocator);

struct QueueFamilyIndices
{
    int graphicsFamily = -1;
    int presentFamily = 1;

    bool isComplete()
    {
        return ((graphicsFamily >= 0) &&
            (presentFamily >= 0));
    }
};

struct SwapChainSupportDetails
{
    VkSurfaceCapabilitiesKHR        capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR>   presentModes;
};

struct Vertex
{
	glm::vec2 pos;
	glm::vec3 color;

	static VkVertexInputBindingDescription GetBindingDescription()
	{
		VkVertexInputBindingDescription bindingDescription = {};
		bindingDescription.binding = 0;
		bindingDescription.stride = sizeof(Vertex);
		bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		return bindingDescription;
	}

	static std::array<VkVertexInputAttributeDescription, 2> GetAttributeDescriptions()
	{
		std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions = {};
		attributeDescriptions[0].binding = 0;
		attributeDescriptions[0].location = 0;
		attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
		attributeDescriptions[0].offset = offsetof(Vertex, pos);

		attributeDescriptions[1].binding = 0;
		attributeDescriptions[1].location = 1;
		attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[1].offset = offsetof(Vertex, color);

		return attributeDescriptions;
	}
};

const std::vector<Vertex> vertices =
{
	{{0.25f, -0.5f},{1.0f, 1.0f, 1.0f}},
	{{0.5f, 0.5f},{0.0f, 1.0f, 0.0f}},
	{{-0.5f, 0.5f},{0.0f, 0.0f, 1.0f}}
};

class HelloTriangleApp
{
public:
    void Run();

private:

    static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugReportFlagsEXT      flags,
                                                        VkDebugReportObjectTypeEXT objType,
                                                        uint64                     obj,
                                                        size_t                     location,
                                                        int32                      code,
                                                        const char*                layerPrefix,
                                                        const char*                msg,
                                                        void*                      userData);
    static std::vector<char>  ReadFile(const std::string& filename);
    static void OnWindowResized(GLFWwindow* pWindow, int width, int height);

    void InitWindow();
    void InitVulkan();
    void MainLoop();
    void Cleanup();
    void CreateInstance();
    bool CheckValidationLayerSupport();
    std::vector<const char*> GetRequiredExtensions();
    void SetupDebugCallback();
    void PickPhysicalDevice();
    QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device);
    bool IsDeviceSuitable(VkPhysicalDevice device);
    bool CreateLogicalDevice();
    void CreateSurface();
    bool CheckDeviceExtensionSupport(VkPhysicalDevice device);
    SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device);
    VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
    VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR> availablePresentModes);
    VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
    void CreateSwapChain();
    void CreateImageViews();
    void CreateGraphicsPipeline();
    VkShaderModule CreateShaderModule(const std::vector<char>& code);
    void CreateRenderPass();
    void CreateFramebuffers();
    void CreateCommandPool();
    void CreateCommandBuffers();
    void DrawFrame();
    void CreateSemaphores();
    void RecreateSwapchain();
    void CleanupSwapchain();
	void CreateVertexBuffer();
	uint32 FindMemoryType(uint32 typeFilter, VkMemoryPropertyFlags properties);

    GLFWwindow*                  m_pWindow;
    VkInstance                   m_VkInstance;               // Handle to the vulkan instance.
    VkDebugReportCallbackEXT     m_hCallback;
    VkPhysicalDevice             m_physicalDevice;
    VkDevice                     m_device;
    VkQueue                      m_graphicsQueue;
    VkQueue                      m_presentQueue;
    VkSurfaceKHR                 m_surface;
    VkSwapchainKHR               m_swapChain;
    std::vector<VkImage>         m_swapChainImages;
    std::vector<VkImageView>     m_swapchainImageViews;
    VkFormat                     m_swapchainImageFormat;
    VkExtent2D                   m_swapchainExtent;
    VkRenderPass                 m_renderPass;
    VkPipelineLayout             m_pipelineLayout;
    VkPipeline                   m_graphicsPipeline;
    std::vector<VkFramebuffer>   m_swapchainFramebuffers;
    VkCommandPool                m_commandPool;
    std::vector<VkCommandBuffer> m_commandBuffers;
    VkSemaphore                  m_imageAvailableSemaphore;
    VkSemaphore                  m_renderFinishedSemaphore;
	VkBuffer					 m_vertexBuffer;
	VkDeviceMemory				 m_vertexBufferMemory;
};