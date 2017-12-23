#define GLFW_INCLUDE_VULKAN
#include <GLFW\glfw3.h>

#include <functional>
#include <iostream>
#include <stdexcept>
#include <vector>

constexpr int WIDTH  = 800;
constexpr int HEIGHT = 600;

//=================================================================================================
class HelloTriangleApp
{
public:
    void Run();
private:
    void InitWindow();
    void InitVulkan();
    void MainLoop();
    void Cleanup();
    void CreateInstance();

    GLFWwindow* m_pWindow;
    VkInstance  m_VkInstance; // Handle to the vulkan instance.
};

//=================================================================================================
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
    vkDestroyInstance(m_VkInstance, nullptr);
    glfwDestroyWindow(m_pWindow);
    glfwTerminate();
}

void HelloTriangleApp::CreateInstance()
{
    // provides info about app/engine to vulkan api.
    VkApplicationInfo appInfo  = { };
    appInfo.sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName   = "Hello Triangle!!";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName        = "wiz engine";
    appInfo.engineVersion      = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion         = VK_API_VERSION_1_0;

    uint32_t glfwExtenstionCount = 0;
    const char** ppGlfwExtensions;

    // we need to get extensions for interfacing with window system since vulkan is platform agnostic
    ppGlfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtenstionCount);

    VkInstanceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;
    createInfo.enabledExtensionCount = glfwExtenstionCount;
    createInfo.ppEnabledExtensionNames = ppGlfwExtensions;
    createInfo.enabledLayerCount = 0;

    VkResult result = vkCreateInstance(&createInfo, nullptr, &m_VkInstance);

    if (result != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create vulkan instance!");
    }

    // enumerate instance extensions
    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

    std::vector<VkExtensionProperties> extensions(extensionCount);

    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

    std::cout << "available extensions: " << std::endl;

    for (const auto& extension : extensions)
    {
        std::cout << "\t" << extension.extensionName << std::endl;
    }
}