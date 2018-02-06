#pragma once

#define GLFW_INCLUDE_VULKAN
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#include <GLFW\glfw3.h>
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <stb_image.h>
#include <tiny_obj_loader.h>

#include <algorithm>
#include <array>
#include <chrono>
#include <functional>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <set>
#include <vector>

#include "common.h"

constexpr int WIDTH  = 800;
constexpr int HEIGHT = 600;
const std::string MODEL_PATH = "../../../assets/chalet.obj";
const std::string TEXTURE_PATH = "../../../assets/chalet.jpg";

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

struct UniformBufferObject
{
	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 proj;
};

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
	glm::vec3 pos;
	glm::vec3 color;
	glm::vec2 texCoord;

	static VkVertexInputBindingDescription GetBindingDescription()
	{
		VkVertexInputBindingDescription bindingDescription = {};
		bindingDescription.binding = 0;
		bindingDescription.stride = sizeof(Vertex);
		bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		return bindingDescription;
	}

	static std::array<VkVertexInputAttributeDescription, 3> GetAttributeDescriptions()
	{
		std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions = {};
		attributeDescriptions[0].binding = 0;
		attributeDescriptions[0].location = 0;
		attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[0].offset = offsetof(Vertex, pos);

		attributeDescriptions[1].binding = 0;
		attributeDescriptions[1].location = 1;
		attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[1].offset = offsetof(Vertex, color);

		attributeDescriptions[2].binding = 0;
		attributeDescriptions[2].location = 2;
		attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
		attributeDescriptions[2].offset = offsetof(Vertex, texCoord);

		return attributeDescriptions;
	}
};


class HelloModel
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
    void CreateSwapChainImageViews();
    void CreateGraphicsPipeline();
    VkShaderModule CreateShaderModule(const std::vector<char>& code);
    void CreateRenderPass();
    void CreateFramebuffers();
    void CreateCommandPool();
    void CreateCommandBuffers();
    void DrawFrame();
	void UpdateUniformBuffer();
    void CreateSemaphores();
    void RecreateSwapchain();
    void CleanupSwapchain();
	void CreateVertexBuffer();
	uint32 FindMemoryType(uint32 typeFilter, VkMemoryPropertyFlags properties);
	void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
	void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
	void CreateIndexBuffer();
	void CreateDescriptorSetLayout();
	void CreateUniformBuffer();
	void CreateDescriptorPool();
	void CreateDescriptorSet();
	void CreateTextureImage();
	void CreateImage(uint32 width, uint32 height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);
	VkCommandBuffer BeginSingleTimeCommands();
	void EndSingleTimeCommands(VkCommandBuffer commandBuffer);
	void TransitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
	void CopyBufferToImage(VkBuffer buffer, VkImage image, uint32 width, uint32 height);
	void CreateTextureImageView();
	VkImageView CreateImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);
	void CreateTextureSampler();
	void CreateDepthResources();
	VkFormat FindSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
	VkFormat FindDepthFormat();
	bool HasStencilComponent(VkFormat format);
	void LoadModel();

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
	VkDescriptorSetLayout		 m_descriptorSetLayout;
    VkPipelineLayout             m_pipelineLayout;
    VkPipeline                   m_graphicsPipeline;
    std::vector<VkFramebuffer>   m_swapchainFramebuffers;
    VkCommandPool                m_commandPool;
    std::vector<VkCommandBuffer> m_commandBuffers;
    VkSemaphore                  m_imageAvailableSemaphore;
    VkSemaphore                  m_renderFinishedSemaphore;
	VkBuffer					 m_vertexBuffer;
	VkDeviceMemory				 m_vertexBufferMemory;
	VkBuffer					 m_indexBuffer;
	VkDeviceMemory				 m_indexBufferMemory;
	VkBuffer				     m_uniformBuffer;
	VkDeviceMemory				 m_uniformBufferMemory;
	VkDescriptorPool			 m_descriptorPool;
	VkDescriptorSet				 m_descriptorSet;
	VkImage						 m_textureImage;
	VkDeviceMemory			     m_textureImageMemory;
	VkImageView					 m_textureImageView;
	VkSampler					 m_textureSampler;
	VkImage						 m_depthImage;
	VkDeviceMemory				 m_depthImageMemory;
	VkImageView					 m_depthImageView;
	std::vector<Vertex>			 m_vertices;
	std::vector<uint32>			 m_indices;
};
