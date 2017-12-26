#define GLFW_INCLUDE_VULKAN
#include <GLFW\glfw3.h>

#include <functional>
#include <iostream>
#include <stdexcept>
#include <vector>

#include "common.h"

constexpr int WIDTH = 800;
constexpr int HEIGHT = 600;

const std::vector<const char*> validationLayers =
{
    "VK_LAYER_LUNARG_standard_validation"
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

//=================================================================================================
// declarations

struct QueueFamilyIndices
{
    int graphicsFamily = -1;

    bool isComplete()
    {
        return graphicsFamily >= 0;
    }
};

class HelloTriangleApp
{
public:
    void Run();

private:

    static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
        VkDebugReportFlagsEXT      flags,
        VkDebugReportObjectTypeEXT objType,
        uint64                     obj,
        size_t                     location,
        int32                      code,
        const char*                layerPrefix,
        const char*                msg,
        void*                      userData);

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


    GLFWwindow* m_pWindow;
    VkInstance  m_VkInstance;               // Handle to the vulkan instance.
    VkDebugReportCallbackEXT m_hCallback;
    VkPhysicalDevice m_physicalDevice;
};

//=================================================================================================
// Main

int main()
{
    HelloTriangleApp app;

    try
    {
        app.Run();
    }
    catch (const std::runtime_error& e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

//=================================================================================================
// Definitions

VkResult CreateDebugReportCallbackEXT(
    VkInstance instance,
    const VkDebugReportCallbackCreateInfoEXT* pCreateInfo,
    const VkAllocationCallbacks*  pAllocator,
    VkDebugReportCallbackEXT* pCallback)
{
    VkResult result = VK_ERROR_EXTENSION_NOT_PRESENT;

    auto func = (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(instance,
        "vkCreateDebugReportCallbackEXT");

    if (func != nullptr)
    {
        result = func(instance, pCreateInfo, pAllocator, pCallback);
    }

    return result;
}

void DestroyDebugReportCallbackEXT(
    VkInstance instance,
    VkDebugReportCallbackEXT callback,
    const VkAllocationCallbacks* pAllocator)
{
    auto func = (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(instance,
        "vkDestroyDebugReportCallbackEXT");
    if (func != nullptr)
    {
        func(instance, callback, pAllocator);
    }
}

VKAPI_ATTR VkBool32 VKAPI_CALL HelloTriangleApp::DebugCallback(
    VkDebugReportFlagsEXT flags,
    VkDebugReportObjectTypeEXT objType,
    uint64 obj,
    size_t location,
    int32 code,
    const char* layerPrefix,
    const char* msg,
    void* userData)
{
    std::cerr << "validation layer: " << msg << std::endl;

    return VK_FALSE;
}

void HelloTriangleApp::Run()
{
    InitWindow();
    InitVulkan();
    MainLoop();
    Cleanup();
}

void HelloTriangleApp::InitWindow()
{
    glfwInit();

    // Glfw loads openGL traditionally. Tell it not to.
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    // resizeable windows are complicated. disable for now.
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    m_pWindow = glfwCreateWindow(WIDTH, HEIGHT, "vulkan draw triangle", nullptr, nullptr);
}


void HelloTriangleApp::InitVulkan()
{
    CreateInstance();
    SetupDebugCallback();
    PickPhysicalDevice();
}

void HelloTriangleApp::MainLoop()
{
    while (!glfwWindowShouldClose(m_pWindow))
    {
        glfwPollEvents();
    }
}

void HelloTriangleApp::Cleanup()
{
    DestroyDebugReportCallbackEXT(m_VkInstance, m_hCallback, nullptr);
    vkDestroyInstance(m_VkInstance, nullptr);
    glfwDestroyWindow(m_pWindow);
    glfwTerminate();
}

void HelloTriangleApp::CreateInstance()
{
    // first check for validation layers support.
    if (enableValidationLayers)
    {
        if (CheckValidationLayerSupport() == false)
        {
            throw std::runtime_error("Validation layers requested, but not available!");
        }
        else
        {
            std::cout << "Validation layers supported." << std::endl;
        }
    }

    // provides info about app/engine to vulkan api.
    VkApplicationInfo appInfo  = { };
    appInfo.sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName   = "Hello Triangle!!";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName        = "wiz engine";
    appInfo.engineVersion      = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion         = VK_API_VERSION_1_0;

    VkInstanceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    auto extensions = GetRequiredExtensions();
    createInfo.enabledExtensionCount = static_cast<uint32>(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.data();

    if (enableValidationLayers)
    {
        createInfo.enabledLayerCount   = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();
    }
    else
    {
        createInfo.enabledLayerCount = 0;
    }
    
    VkResult result = vkCreateInstance(&createInfo, nullptr, &m_VkInstance);

    if (result != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create vulkan instance!");
    }
}

bool HelloTriangleApp::CheckValidationLayerSupport()
{
    // false if any layer in the validationLayers is not supported.
    bool layerFound = false;

    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    for (const char* layerName : validationLayers)
    {
        for (const auto& layerProperties : availableLayers)
        {
            if (strcmp(layerName, layerProperties.layerName) == 0)
            {
                layerFound = true;
                break;
            }
        }
    }

    return layerFound;
}

std::vector<const char*> HelloTriangleApp::GetRequiredExtensions()
{
    std::vector<const char*> extensions;

    uint32 glfwExtensionCount = 0;
    const char** glfwExtensions;

    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    for (uint32 i = 0; i < glfwExtensionCount; i++)
    {
        extensions.push_back(glfwExtensions[i]);
    }

    if (enableValidationLayers)
    {
        extensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
    }


    std::cout << "glfw requested extensions: " << std::endl;

    for (const auto& extension : extensions)
    {
        std::cout << "\t" << extension << std::endl;
    }

    return extensions;
}



void HelloTriangleApp::SetupDebugCallback()
{
    if (enableValidationLayers)
    {
        VkDebugReportCallbackCreateInfoEXT createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
        createInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT;
        createInfo.pfnCallback = DebugCallback;

        VkResult result = CreateDebugReportCallbackEXT(m_VkInstance,
                                                       &createInfo,
                                                       nullptr,
                                                       &m_hCallback);
        if (result != VK_SUCCESS)
        {
            throw std::runtime_error("failed to setup debug callback!");
        }
    }
}



void HelloTriangleApp::PickPhysicalDevice()
{
    m_physicalDevice = VK_NULL_HANDLE;
    uint32 deviceCount = 0;
    vkEnumeratePhysicalDevices(m_VkInstance, &deviceCount, nullptr);

    if (deviceCount == 0)
    {
        throw std::runtime_error("failed to find GPUs with vulkan support!");
    }

    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(m_VkInstance, &deviceCount, devices.data());

    for (const auto& device : devices)
    {
        if (IsDeviceSuitable(device))
        {
            m_physicalDevice = device;
            break;
        }
    }

    if (m_physicalDevice == VK_NULL_HANDLE)
    {
        throw std::runtime_error("failed to find a suitable GPU!");
    }
}

QueueFamilyIndices HelloTriangleApp::FindQueueFamilies(
    VkPhysicalDevice device)
{
    QueueFamilyIndices indices;
    uint32 queueFamilyCount = 0;

    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

    int i = 0;
    for (const auto& queueFamily : queueFamilies)
    {
        if ((queueFamily.queueCount > 0) &&
            (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT))
        {
            indices.graphicsFamily = i;
        }

        if (indices.isComplete())
        {
            break;
        }

        i++;
    }

    return indices;
}

bool HelloTriangleApp::IsDeviceSuitable(
    VkPhysicalDevice device)
{
    /*    VkPhysicalDeviceProperties deviceProperties;
    vkGetPhysicalDeviceProperties(device, &deviceProperties);

    VkPhysicalDeviceFeatures deviceFeatures;
    vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

    return deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU &&
    deviceFeatures.geometryShader;
    */

    QueueFamilyIndices indices = FindQueueFamilies(device);

    return indices.isComplete();
}