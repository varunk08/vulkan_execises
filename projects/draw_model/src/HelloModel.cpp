#include "HelloModel.h"

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

VKAPI_ATTR VkBool32 VKAPI_CALL HelloModel::DebugCallback(
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

// helper function to shader binary files
// reads all the bytes and returns a byte array managed by std::vector
std::vector<char> HelloModel::ReadFile(
    const std::string& filename)
{
    // ate: start reading at the end of the file to find size
    // binary: read as binary
    std::ifstream file(filename, std::ios::ate | std::ios::binary);

    if (file.is_open() == false)
    {
        throw std::runtime_error("failed to open file!");
    }

    size_t fileSize = static_cast<size_t>(file.tellg());
    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), fileSize);
    file.close();

    return buffer;
}

void HelloModel::OnWindowResized(
    GLFWwindow* pWindow,
    int         width,
    int         height)
{
    if (width == 0 || height == 0)
    {
        return;
    }

    HelloModel* pApp = reinterpret_cast<HelloModel*>(glfwGetWindowUserPointer(pWindow));
    pApp->RecreateSwapchain();
}

void HelloModel::Run()
{
    InitWindow();
    InitVulkan();
    MainLoop();
    Cleanup();
}

void HelloModel::InitWindow()
{
    glfwInit();

    // Glfw loads openGL traditionally. Tell it not to.
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    
    m_pWindow = glfwCreateWindow(WIDTH, HEIGHT, "Draw Obj model - Vulkan", nullptr, nullptr);

    glfwSetWindowUserPointer(m_pWindow, this);
    glfwSetWindowSizeCallback(m_pWindow, HelloModel::OnWindowResized);
}


void HelloModel::InitVulkan()
{
    CreateInstance();
    SetupDebugCallback();
    CreateSurface();

	// device
    PickPhysicalDevice();
    CreateLogicalDevice();

	// swap chain, swap chain image views
    CreateSwapChain();
    CreateSwapChainImageViews();

	// render pass
    CreateRenderPass();
	CreateDescriptorSetLayout();

	//  pipeline, command allocator
    CreateGraphicsPipeline();
    CreateCommandPool();

	// texture images, depth buffers, frame buffers,
	CreateDepthResources();
    CreateFramebuffers();		// create frame buffers after the depth buffer have been created
	CreateTextureImage();
	CreateTextureImageView();
	CreateTextureSampler();
	
	// vertex buffers, index buffers, uniform buffers
	LoadModel();
	CreateVertexBuffer();
	CreateIndexBuffer();
	CreateUniformBuffer();

	// descriptors
	CreateDescriptorPool();
	CreateDescriptorPool();
	CreateDescriptorSet();
	
	CreateCommandBuffers();

    CreateSemaphores();
}

void HelloModel::MainLoop()
{
    while (!glfwWindowShouldClose(m_pWindow))
    {
        glfwPollEvents();

		UpdateUniformBuffer();
        DrawFrame();
    }

    vkDeviceWaitIdle(m_device);
}

void HelloModel::Cleanup()
{
	CleanupSwapchain();
	
	// texture sampler
	vkDestroySampler(m_device, m_textureSampler, nullptr);
	
	// texture image
	vkDestroyImageView(m_device, m_textureImageView, nullptr);
	vkDestroyImage(m_device, m_textureImage, nullptr);
	vkFreeMemory(m_device, m_textureImageMemory, nullptr);
	
	// descriptor pool
	vkDestroyDescriptorPool(m_device, m_descriptorPool, nullptr);
	
	// descriptor set layout
	vkDestroyDescriptorSetLayout(m_device, m_descriptorSetLayout, nullptr);


	// uniform buffer
	vkDestroyBuffer(m_device, m_uniformBuffer, nullptr);
	vkFreeMemory(m_device, m_uniformBufferMemory, nullptr);
	
	// index buffer
	vkDestroyBuffer(m_device, m_indexBuffer, nullptr);
	vkFreeMemory(m_device, m_indexBufferMemory, nullptr);

	// vertex buffer
	vkDestroyBuffer(m_device, m_vertexBuffer, nullptr);
	vkFreeMemory(m_device, m_vertexBufferMemory, nullptr);

    vkDestroySemaphore(m_device, m_renderFinishedSemaphore, nullptr);
    vkDestroySemaphore(m_device, m_imageAvailableSemaphore, nullptr);

	// command pool
    vkDestroyCommandPool(m_device, m_commandPool, nullptr);

    vkDestroyDevice(m_device, nullptr);
    DestroyDebugReportCallbackEXT(m_VkInstance, m_hCallback, nullptr);
    vkDestroySurfaceKHR(m_VkInstance, m_surface, nullptr);
    vkDestroyInstance(m_VkInstance, nullptr);
	
    glfwDestroyWindow(m_pWindow);
    glfwTerminate();
}

void HelloModel::CreateInstance()
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
    VkApplicationInfo appInfo = {};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Hello Triangle!!";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "wiz engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    auto extensions = GetRequiredExtensions();
    createInfo.enabledExtensionCount = static_cast<uint32>(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.data();

    if (enableValidationLayers)
    {
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
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

bool HelloModel::CheckValidationLayerSupport()
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

std::vector<const char*> HelloModel::GetRequiredExtensions()
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



void HelloModel::SetupDebugCallback()
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



void HelloModel::PickPhysicalDevice()
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

QueueFamilyIndices HelloModel::FindQueueFamilies(
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
        if (queueFamily.queueCount > 0)
        {
            if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
            {
                indices.graphicsFamily = i;
            }

            VkBool32 presentSupport = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(device, i, m_surface, &presentSupport);

            if (presentSupport)
            {
                indices.presentFamily = i;
            }
        }

        if (indices.isComplete())
        {
            break;
        }

        i++;
    }

    return indices;
}

bool HelloModel::IsDeviceSuitable(
    VkPhysicalDevice device)
{
    QueueFamilyIndices indices = FindQueueFamilies(device);

    bool extensionsSupported = CheckDeviceExtensionSupport(device);

    bool swapChainAdequate = false;
    if (extensionsSupported)
    {
        SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(device);
        swapChainAdequate = (swapChainSupport.formats.empty() == false) &&
            (swapChainSupport.presentModes.empty() == false);
    }

	VkPhysicalDeviceFeatures supportedFeatures;
	vkGetPhysicalDeviceFeatures(device, &supportedFeatures);

	return indices.isComplete() &&
		extensionsSupported &&
		swapChainAdequate &&
		supportedFeatures.samplerAnisotropy;
}

bool HelloModel::CreateLogicalDevice()
{
    QueueFamilyIndices indices = FindQueueFamilies(m_physicalDevice);

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<int> uniqueQueueFamilies = { indices.graphicsFamily, indices.presentFamily };

    float queuePriority = 1.0f;

    for (int queueFamily : uniqueQueueFamilies)
    {
        VkDeviceQueueCreateInfo queueCreateInfo = {};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;

        queueCreateInfos.push_back(queueCreateInfo);
    }


    VkPhysicalDeviceFeatures deviceFeatures = {};
	deviceFeatures.samplerAnisotropy	= VK_TRUE;
	deviceFeatures.depthBounds			= VK_TRUE;

    VkDeviceCreateInfo createInfo = {};
    createInfo.sType					= VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.pQueueCreateInfos		= queueCreateInfos.data();
    createInfo.queueCreateInfoCount		= static_cast<uint32>(queueCreateInfos.size());
    createInfo.pEnabledFeatures			= &deviceFeatures;
    createInfo.enabledExtensionCount	= static_cast<uint32>(deviceExtensions.size());
    createInfo.ppEnabledExtensionNames	= deviceExtensions.data();

    if (enableValidationLayers)
    {
        createInfo.enabledLayerCount = static_cast<uint32>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();
    }
    else
    {
        createInfo.enabledLayerCount = 0;
    }

    VkResult result = vkCreateDevice(m_physicalDevice, &createInfo, nullptr, &m_device);

    if (result != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create logical device!");
    }

    vkGetDeviceQueue(m_device, indices.graphicsFamily, 0, &m_graphicsQueue);
    vkGetDeviceQueue(m_device, indices.presentFamily, 0, &m_presentQueue);

}

void HelloModel::CreateSurface()
{
    VkResult result = glfwCreateWindowSurface(m_VkInstance, m_pWindow, nullptr, &m_surface);

    if (result != VK_SUCCESS)
    {
        throw std::runtime_error("failed to crete window surface!");
    }
}

bool HelloModel::CheckDeviceExtensionSupport(
    VkPhysicalDevice device)
{
    uint32 extensionCount;

    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

    std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

    for (const auto& extension : availableExtensions)
    {
        requiredExtensions.erase(extension.extensionName);
    }

    return requiredExtensions.empty();
}

SwapChainSupportDetails HelloModel::QuerySwapChainSupport(
    VkPhysicalDevice device)
{
    SwapChainSupportDetails details;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, m_surface, &details.capabilities);

    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_surface, &formatCount, nullptr);

    if (formatCount != 0)
    {
        details.formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_surface, &formatCount, details.formats.data());
    }

    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_surface, &presentModeCount, nullptr);

    if (presentModeCount != 0)
    {
        details.presentModes.resize(presentModeCount);

        vkGetPhysicalDeviceSurfacePresentModesKHR(device,
            m_surface,
            &presentModeCount,
            details.presentModes.data());
    }

    return details;
}

VkSurfaceFormatKHR HelloModel::ChooseSwapSurfaceFormat(
    const std::vector<VkSurfaceFormatKHR>& availableFormats)
{
    if ((availableFormats.size() == 1) &&
        (availableFormats[0].format == VK_FORMAT_UNDEFINED))
    {
        return{ VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
    }

    for (const auto& availableFormat : availableFormats)
    {
        if ((availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM) &&
            (availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR))
        {
            return availableFormat;
        }
    }

    return availableFormats[0];
}

VkPresentModeKHR HelloModel::ChooseSwapPresentMode(
    const std::vector<VkPresentModeKHR> availablePresentModes)
{
    VkPresentModeKHR bestMode = VK_PRESENT_MODE_FIFO_KHR;

    for (const auto& availablePresentMode : availablePresentModes)
    {
        if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
        {
            bestMode = availablePresentMode;
            break;
        }
        else if (availablePresentMode == VK_PRESENT_MODE_IMMEDIATE_KHR)
        {
            bestMode = availablePresentMode;
        }

    }

    return bestMode;
}


VkExtent2D HelloModel::ChooseSwapExtent(
    const VkSurfaceCapabilitiesKHR& capabilities)
{
    if (capabilities.currentExtent.width != std::numeric_limits<uint32>::max())
    {
        return capabilities.currentExtent;
    }
    else
    {
        int width, height;
        glfwGetWindowSize(m_pWindow, &width, &height);

        VkExtent2D actualExtent = { width, height };
        actualExtent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actualExtent.width));
        actualExtent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actualExtent.height));

        return actualExtent;
    }
}

void HelloModel::CreateSwapChain()
{
    SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(m_physicalDevice);

    VkSurfaceFormatKHR surfaceFormat = ChooseSwapSurfaceFormat(swapChainSupport.formats);
    VkPresentModeKHR presentMode = ChooseSwapPresentMode(swapChainSupport.presentModes);
    VkExtent2D extent = ChooseSwapExtent(swapChainSupport.capabilities);


    uint32 imageCount = swapChainSupport.capabilities.minImageCount + 1;

    if ((swapChainSupport.capabilities.maxImageCount > 0) &&
        (imageCount > swapChainSupport.capabilities.maxImageCount))
    {
        imageCount = swapChainSupport.capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = m_surface;
    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    QueueFamilyIndices indices = FindQueueFamilies(m_physicalDevice);
    uint32 queueFamilyIndices[] = { static_cast<uint32>(indices.graphicsFamily), static_cast<uint32>(indices.presentFamily) };

    if (indices.graphicsFamily != indices.presentFamily)
    {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    }
    else
    {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 0;
        createInfo.pQueueFamilyIndices = nullptr;
    }

    createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;
    createInfo.oldSwapchain = VK_NULL_HANDLE;

    VkResult result = vkCreateSwapchainKHR(m_device, &createInfo, nullptr, &m_swapChain);

    if (result != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create swap chain!");
    }

    vkGetSwapchainImagesKHR(m_device, m_swapChain, &imageCount, nullptr);
    m_swapChainImages.resize(imageCount);
    vkGetSwapchainImagesKHR(m_device, m_swapChain, &imageCount, m_swapChainImages.data());

    m_swapchainImageFormat = surfaceFormat.format;
    m_swapchainExtent = extent;
}

// Creates a basic image view for every image in the swap chain for use as color targets.
void HelloModel::CreateSwapChainImageViews()
{
    m_swapchainImageViews.resize(m_swapChainImages.size());

    for (size_t i = 0; i < m_swapChainImages.size(); ++i)
    {
		m_swapchainImageViews[i] = CreateImageView(m_swapChainImages[i], m_swapchainImageFormat, VK_IMAGE_ASPECT_COLOR_BIT);
    }

}

void HelloModel::CreateGraphicsPipeline()
{
    // visual studio current directory is the same as the project directory!
    auto vertShaderCode = ReadFile("../shaders/vert.spv");
    auto fragShaderCode = ReadFile("../shaders/frag.spv");

    VkShaderModule vertShaderModule = CreateShaderModule(vertShaderCode);
    VkShaderModule fragShaderModule = CreateShaderModule(fragShaderCode);

    VkPipelineShaderStageCreateInfo vertShaderStageInfo = {};
    vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertShaderStageInfo.module = vertShaderModule;
    vertShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo fragShaderStageInfo = {};
    fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragShaderStageInfo.module = fragShaderModule;
    fragShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

    // describes the format of the vertex data that will be passed to the vertex shader
	auto bindingDescription = Vertex::GetBindingDescription();
	auto  attributeDescriptions = Vertex::GetAttributeDescriptions();

	VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexBindingDescriptionCount = 1;
	vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
    vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32>(attributeDescriptions.size());
	vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

                                                            // describes what kind of geometry will be drawn and if primitive restart should be enabled
    VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssembly.primitiveRestartEnable = VK_FALSE;

    VkViewport viewport = {};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float)m_swapchainExtent.width;
    viewport.height = (float)m_swapchainExtent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor = {};
    scissor.offset = { 0, 0 };
    scissor.extent = m_swapchainExtent;

    VkPipelineViewportStateCreateInfo viewportState = {};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.pViewports = &viewport;
    viewportState.scissorCount = 1;
    viewportState.pScissors = &scissor;

    VkPipelineRasterizationStateCreateInfo rasterizer = {};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterizer.depthBiasEnable = VK_FALSE;
    rasterizer.depthBiasConstantFactor = 0.0f; // Optional
    rasterizer.depthBiasClamp = 0.0f; // Optional
    rasterizer.depthBiasSlopeFactor = 0.0f; // Optional

    VkPipelineMultisampleStateCreateInfo multisampling = {};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    multisampling.minSampleShading = 1.0f; // Optional
    multisampling.pSampleMask = nullptr; // Optional
    multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
    multisampling.alphaToOneEnable = VK_FALSE; // Optional

    VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_FALSE;
    colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
    colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
    colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD; // Optional
    colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
    colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
    colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD; // Optional
    colorBlendAttachment.blendEnable = VK_TRUE;
    colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
    colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

    VkPipelineColorBlendStateCreateInfo colorBlending = {};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &colorBlendAttachment;
    colorBlending.blendConstants[0] = 0.0f; // Optional
    colorBlending.blendConstants[1] = 0.0f; // Optional
    colorBlending.blendConstants[2] = 0.0f; // Optional
    colorBlending.blendConstants[3] = 0.0f; // Optional

    VkDynamicState dynamicStates[] = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_LINE_WIDTH
    };

    VkPipelineDynamicStateCreateInfo dynamicState = {};
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.dynamicStateCount = 2;
    dynamicState.pDynamicStates = dynamicStates;

    // dummy pipeline layout. no uniform variables in this example
    VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = 1;
	pipelineLayoutInfo.pSetLayouts = &m_descriptorSetLayout;
    pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
    pipelineLayoutInfo.pPushConstantRanges = 0; // Optional

    if (vkCreatePipelineLayout(m_device, &pipelineLayoutInfo, nullptr, &m_pipelineLayout) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create pipeline layout!");
    }

	VkPipelineDepthStencilStateCreateInfo depthStencil = {};
	depthStencil.sType						= VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	depthStencil.depthTestEnable			= VK_TRUE;
	depthStencil.depthWriteEnable			= VK_TRUE;
	depthStencil.depthCompareOp				= VK_COMPARE_OP_LESS;
	depthStencil.depthBoundsTestEnable		= VK_TRUE;
	depthStencil.minDepthBounds				= 0.0f;
	depthStencil.maxDepthBounds				= 1.0f;
	depthStencil.stencilTestEnable			= VK_FALSE;
	depthStencil.front						= {};
	depthStencil.back						= {};


    VkGraphicsPipelineCreateInfo pipelineInfo = {};
    pipelineInfo.sType						= VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount					= 2;
    pipelineInfo.pStages					= shaderStages;
    pipelineInfo.pVertexInputState			= &vertexInputInfo;
    pipelineInfo.pInputAssemblyState		= &inputAssembly;
    pipelineInfo.pViewportState				= &viewportState;
    pipelineInfo.pRasterizationState		= &rasterizer;
    pipelineInfo.pMultisampleState			= &multisampling;
    pipelineInfo.pDepthStencilState			= &depthStencil;
    pipelineInfo.pColorBlendState			= &colorBlending;
    pipelineInfo.pDynamicState				= nullptr; // Optional
    pipelineInfo.layout						= m_pipelineLayout;
    pipelineInfo.renderPass					= m_renderPass;
    pipelineInfo.subpass					= 0;
    pipelineInfo.basePipelineHandle			= VK_NULL_HANDLE; // Optional
    pipelineInfo.basePipelineIndex			= -1; // Optional

    VkResult result = vkCreateGraphicsPipelines(m_device,
        VK_NULL_HANDLE,
        1,
        &pipelineInfo,
        nullptr,
        &m_graphicsPipeline);
    if (result != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create graphics pipeline!");
    }

    vkDestroyShaderModule(m_device, fragShaderModule, nullptr);
    vkDestroyShaderModule(m_device, vertShaderModule, nullptr);
}


// creates a shader module from bytecode
VkShaderModule HelloModel::CreateShaderModule(
    const std::vector<char>& code)
{
    VkShaderModuleCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = code.size();

    // this bytecode pointer expects a uint32 pointer rather than a char pointer so need to reinterpret_cast
    createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

    VkShaderModule shaderModule;

    VkResult result = vkCreateShaderModule(m_device, &createInfo, nullptr, &shaderModule);

    if (result != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create shader module!");
    }

    return shaderModule;
}

void HelloModel::CreateRenderPass()
{
    VkAttachmentDescription colorAttachment = {};
    colorAttachment.format = m_swapchainImageFormat;
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference colorAttachmentRef = {};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkAttachmentDescription depthAttachment = {};
	depthAttachment.format			= FindDepthFormat();
	depthAttachment.samples			= VK_SAMPLE_COUNT_1_BIT;
	depthAttachment.loadOp			= VK_ATTACHMENT_LOAD_OP_CLEAR;
	depthAttachment.storeOp			= VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment.stencilLoadOp	= VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	depthAttachment.stencilStoreOp  = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment.initialLayout	= VK_IMAGE_LAYOUT_UNDEFINED;
	depthAttachment.finalLayout		= VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkAttachmentReference depthAttachRef = {};
	depthAttachRef.attachment			 = 1;
	depthAttachRef.layout				 = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass = {};
    subpass.pipelineBindPoint		= VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount	= 1;
    subpass.pColorAttachments		= &colorAttachmentRef;
	subpass.pDepthStencilAttachment = &depthAttachRef;

    VkSubpassDependency dependency = {};
    dependency.srcSubpass    = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass    = 0;
    dependency.srcStageMask  = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask  = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

	std::array<VkAttachmentDescription, 2> attachments = { colorAttachment, depthAttachment };

    VkRenderPassCreateInfo renderPassInfo = {};
    renderPassInfo.sType				= VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount		= static_cast<uint32>(attachments.size());
    renderPassInfo.pAttachments			= attachments.data();
    renderPassInfo.subpassCount			= 1;
    renderPassInfo.pSubpasses			= &subpass;
    renderPassInfo.dependencyCount		= 1;
    renderPassInfo.pDependencies		= &dependency;

    if (vkCreateRenderPass(m_device, &renderPassInfo, nullptr, &m_renderPass) != VK_SUCCESS) {
        throw std::runtime_error("failed to create render pass!");
    }
}

void HelloModel::CreateFramebuffers()
{
    m_swapchainFramebuffers.resize(m_swapchainImageViews.size());
	
	std::array<VkImageView, 2> attachments = {};

    for (size_t i = 0; i < m_swapchainImageViews.size(); i++)
    {
		attachments = { m_swapchainImageViews[i], m_depthImageView };
		
        VkFramebufferCreateInfo framebufferInfo = {};
        framebufferInfo.sType				= VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass			= m_renderPass;
        framebufferInfo.attachmentCount		= static_cast<uint32>(attachments.size());
        framebufferInfo.pAttachments		= attachments.data();
        framebufferInfo.width				= m_swapchainExtent.width;
        framebufferInfo.height				= m_swapchainExtent.height;
        framebufferInfo.layers			    = 1;

        if (vkCreateFramebuffer(m_device, &framebufferInfo, nullptr, &m_swapchainFramebuffers[i]) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create framebuffer!");
        }
    }
}

void HelloModel::CreateCommandPool()
{
    QueueFamilyIndices queueFamilyIndices = FindQueueFamilies(m_physicalDevice);

    VkCommandPoolCreateInfo poolInfo = {};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily;
    poolInfo.flags = 0; // Optional

    if (vkCreateCommandPool(m_device, &poolInfo, nullptr, &m_commandPool) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create command pool!");
    }
}

// Create a command buffer for each swap chain image because the right framebuffer must be specified in one of
// the commands
void HelloModel::CreateCommandBuffers()
{
    m_commandBuffers.resize(m_swapchainFramebuffers.size());

    VkCommandBufferAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = m_commandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = (uint32_t)m_commandBuffers.size();

    if (vkAllocateCommandBuffers(m_device, &allocInfo, m_commandBuffers.data()) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to allocate command buffers!");
    }

	std::array<VkClearValue, 2>  clearValues = {};
	clearValues[0].color		= { 0.0f, 0.0f, 0.0f, 1.0f };
	clearValues[1].depthStencil = { 1.0f, 0 };

    // start command buffer recording
    for (size_t i = 0; i < m_commandBuffers.size(); i++)
    {
        VkCommandBufferBeginInfo beginInfo = {};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
        beginInfo.pInheritanceInfo = nullptr; // Optional

        vkBeginCommandBuffer(m_commandBuffers[i], &beginInfo);

        VkRenderPassBeginInfo renderPassInfo = {};
        renderPassInfo.sType					= VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass				= m_renderPass;
        renderPassInfo.framebuffer				= m_swapchainFramebuffers[i];
        renderPassInfo.renderArea.offset		= { 0, 0 };
        renderPassInfo.renderArea.extent		= m_swapchainExtent;
        renderPassInfo.clearValueCount			= static_cast<uint32>(clearValues.size());
        renderPassInfo.pClearValues				= clearValues.data();

        vkCmdBeginRenderPass(m_commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        vkCmdBindPipeline(m_commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, m_graphicsPipeline);

		VkBuffer vertexBuffers[] = { m_vertexBuffer };
		VkDeviceSize offsets[] = { 0 };

		// bind buffers
		vkCmdBindVertexBuffers(m_commandBuffers[i], 0, 1, vertexBuffers, offsets);
		vkCmdBindIndexBuffer(m_commandBuffers[i], m_indexBuffer, 0, VK_INDEX_TYPE_UINT32);
		vkCmdBindDescriptorSets(m_commandBuffers[i],
								VK_PIPELINE_BIND_POINT_GRAPHICS,
								m_pipelineLayout,
								0,
								1,
								&m_descriptorSet,
								0,
								nullptr);

		vkCmdDrawIndexed(m_commandBuffers[i], static_cast<uint32>(m_indices.size()), 1, 0, 0, 0);
        vkCmdEndRenderPass(m_commandBuffers[i]);

        if (vkEndCommandBuffer(m_commandBuffers[i]) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to record command buffer!");
        }
    }
}

// acquire an image from the swap chain
// execute the command buffer with that image as attachment in the framebuffer
// return the image to the swap chain for presentation
void HelloModel::DrawFrame()
{
    uint32 imageIndex;

    VkResult result = vkAcquireNextImageKHR(m_device,
                                            m_swapChain,
                                            std::numeric_limits<uint64>::max(),
                                            m_imageAvailableSemaphore,
                                            VK_NULL_HANDLE,
                                            &imageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR)
    {
        RecreateSwapchain();
        return;
    }
    else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
    {
        throw std::runtime_error("failed to acquire swap chain image!");
    }

    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = { m_imageAvailableSemaphore };
    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &m_commandBuffers[imageIndex];

    VkSemaphore signalSemaphores[] = { m_renderFinishedSemaphore };
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    if (vkQueueSubmit(m_graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to submit draw command buffer!");
    }

    VkPresentInfoKHR presentInfo = {};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    VkSwapchainKHR swapChains[] = { m_swapChain };
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &imageIndex;
    presentInfo.pResults = nullptr; // Optional

    result = vkQueuePresentKHR(m_presentQueue, &presentInfo);
    
    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
    {
        RecreateSwapchain();
    }
    else if (result != VK_SUCCESS)
    {
        throw std::runtime_error("failed to acquire swap chain image!");
    }

    vkQueueWaitIdle(m_presentQueue);
}

void HelloModel::UpdateUniformBuffer()
{
	static auto startTime = std::chrono::high_resolution_clock::now();

	auto currentTime = std::chrono::high_resolution_clock::now();
	float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

	UniformBufferObject ubo = {};
	ubo.model = glm::rotate(glm::mat4(1.0f), // identity matrix
							time * glm::radians(45.0f), // rotation amount
							glm::vec3(0.0f, 0.0f, 1.0f)); // axis of rotation

	ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), // eye pos
						   glm::vec3(0.0f, 0.0f, 0.0f), // center pos. target?
						   glm::vec3(0.0f, 0.0f, 1.0f));// up axis

	ubo.proj = glm::perspective(glm::radians(45.0f), // fov
								m_swapchainExtent.width / static_cast<float>(m_swapchainExtent.height), // aspect ratio
								0.1f, // near Z
								10.0f); // far Z

	// glm was originally designed for OpenGL so the Y coordinate of the clip coordinates is inverted.
	// flipping the sign on the scaling factor of the Y axis in the projection matrix to compensate.
	ubo.proj[1][1] *= -1; 
	
	void* pData;
	vkMapMemory(m_device, m_uniformBufferMemory, 0, sizeof(ubo), 0, &pData);

	memcpy(pData, &ubo, sizeof(ubo));

	vkUnmapMemory(m_device, m_uniformBufferMemory);
}

void HelloModel::CreateSemaphores()
{
    VkSemaphoreCreateInfo semaphoreInfo = {};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    if (vkCreateSemaphore(m_device, &semaphoreInfo, nullptr, &m_imageAvailableSemaphore) != VK_SUCCESS ||
        vkCreateSemaphore(m_device, &semaphoreInfo, nullptr, &m_renderFinishedSemaphore) != VK_SUCCESS)
    {

        throw std::runtime_error("failed to create semaphores!");
    }
}

void HelloModel::RecreateSwapchain()
{
    vkDeviceWaitIdle(m_device);

    CleanupSwapchain();

    CreateSwapChain();
    CreateSwapChainImageViews();
    CreateRenderPass();
    CreateGraphicsPipeline();
	CreateDepthResources();
    CreateFramebuffers();
    CreateCommandBuffers();
}

void HelloModel::CleanupSwapchain()
{
    for (auto framebuffer : m_swapchainFramebuffers)
    {
        vkDestroyFramebuffer(m_device, framebuffer, nullptr);
    }
    vkFreeCommandBuffers(m_device,
        m_commandPool,
        static_cast<uint32>(m_commandBuffers.size()),
        m_commandBuffers.data());

    vkDestroyPipeline(m_device, m_graphicsPipeline, nullptr);
    vkDestroyPipelineLayout(m_device, m_pipelineLayout, nullptr);
    vkDestroyRenderPass(m_device, m_renderPass, nullptr);

    for (auto imageView : m_swapchainImageViews)
    {
        vkDestroyImageView(m_device, imageView, nullptr);
    }
    vkDestroySwapchainKHR(m_device, m_swapChain, nullptr);

}

void HelloModel::CreateVertexBuffer()
{
	VkDeviceSize bufferSize = sizeof(m_vertices[0]) * m_vertices.size();
	
	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	
	CreateBuffer(bufferSize,
				 VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
				 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
				 stagingBuffer,
				 stagingBufferMemory);
	void* data;
	vkMapMemory(m_device, stagingBufferMemory, 0, bufferSize, 0, &data);
	
	memcpy(data, m_vertices.data(), static_cast<size_t>(bufferSize));
	vkUnmapMemory(m_device, stagingBufferMemory);

	CreateBuffer(bufferSize,
				 VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
				 VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
				 m_vertexBuffer,
				 m_vertexBufferMemory);

	CopyBuffer(stagingBuffer, m_vertexBuffer, bufferSize);

	vkDestroyBuffer(m_device, stagingBuffer, nullptr);
	vkFreeMemory(m_device, stagingBufferMemory, nullptr);
}

uint32 HelloModel::FindMemoryType(
	uint32 typeFilter,
	VkMemoryPropertyFlags properties)
{
	VkPhysicalDeviceMemoryProperties memProperties;
	vkGetPhysicalDeviceMemoryProperties(m_physicalDevice, &memProperties);

	for (uint32 i = 0; i < memProperties.memoryTypeCount; i++)
	{
		if ((typeFilter & (1 << i)) &&
			(memProperties.memoryTypes[i].propertyFlags & properties) == properties)
		{
			return i;
		}
	}

	throw std::runtime_error("failed to find suitable memory type!");
}

void HelloModel::CreateBuffer(
	VkDeviceSize		  size,
	VkBufferUsageFlags	  usage,
	VkMemoryPropertyFlags properties,
	VkBuffer&			  buffer,
	VkDeviceMemory&		  bufferMemory)
{
	VkBufferCreateInfo bufferInfo = {};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = size;
	bufferInfo.usage = usage;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	VkResult result = vkCreateBuffer(m_device, &bufferInfo, nullptr, &buffer);

	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create buffer!");
	}

	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(m_device, buffer, &memRequirements);

	VkMemoryAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = FindMemoryType(memRequirements.memoryTypeBits, properties);

	result = vkAllocateMemory(m_device, &allocInfo, nullptr, &bufferMemory);

	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("failed to allocate buffer memory!");
	}

	vkBindBufferMemory(m_device, buffer, bufferMemory, 0);
}

void HelloModel::CopyBuffer(
	VkBuffer srcBuffer,
	VkBuffer dstBuffer,
	VkDeviceSize size)
{
	VkCommandBuffer commandBuffer = BeginSingleTimeCommands();

	VkBufferCopy copyRegion = {};
	copyRegion.size = size;
	vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

	EndSingleTimeCommands(commandBuffer);
}

void HelloModel::CreateIndexBuffer()
{
	VkDeviceSize bufferSize = sizeof(m_indices[0]) * m_indices.size();

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;

	CreateBuffer(bufferSize,
				 VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
				 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
				 stagingBuffer,
				 stagingBufferMemory);

	void* pData;
	vkMapMemory(m_device, stagingBufferMemory, 0, bufferSize, 0, &pData);
	memcpy(pData, m_indices.data(), static_cast<size_t>(bufferSize));
	vkUnmapMemory(m_device, stagingBufferMemory);

	CreateBuffer(bufferSize,
				 VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
				 VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
				 m_indexBuffer,
				 m_indexBufferMemory);

	CopyBuffer(stagingBuffer, m_indexBuffer, bufferSize);

	vkDestroyBuffer(m_device, stagingBuffer, nullptr);
	vkFreeMemory(m_device, stagingBufferMemory, nullptr);
}

void HelloModel::CreateDescriptorSetLayout()
{
	VkDescriptorSetLayoutBinding samplerLayoutBinding = {};
	samplerLayoutBinding.binding = 1;
	samplerLayoutBinding.descriptorCount = 1;
	samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	samplerLayoutBinding.pImmutableSamplers = nullptr;
	samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	VkDescriptorSetLayoutBinding uboLayoutBinding = {};
	uboLayoutBinding.binding = 0;
	uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	uboLayoutBinding.descriptorCount = 1;
	uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	uboLayoutBinding.pImmutableSamplers = nullptr;

	std::array<VkDescriptorSetLayoutBinding, 2> bindings = { uboLayoutBinding, samplerLayoutBinding };

	VkDescriptorSetLayoutCreateInfo layoutInfo = {};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = static_cast<uint32>(bindings.size());
	layoutInfo.pBindings = bindings.data();

	VkResult result = vkCreateDescriptorSetLayout(m_device, &layoutInfo, nullptr, &m_descriptorSetLayout);

	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create descriptor set layout!");
	} 	
}


void HelloModel::CreateUniformBuffer()
{
	VkDeviceSize bufferSize = sizeof(UniformBufferObject);
	CreateBuffer(bufferSize,
				 VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
				 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
				 m_uniformBuffer,
				 m_uniformBufferMemory);

}

void HelloModel::CreateDescriptorPool()
{
	std::array<VkDescriptorPoolSize, 2> poolSizes = {};
	poolSizes[0].type							  = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSizes[0].descriptorCount				  = 1;
	poolSizes[1].type							  = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	poolSizes[1].descriptorCount				  = 1;

	VkDescriptorPoolCreateInfo poolInfo = {};
	poolInfo.sType						= VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount				= static_cast<uint32>(poolSizes.size());
	poolInfo.pPoolSizes					= poolSizes.data();
	poolInfo.maxSets					= 1;

	VkResult result = vkCreateDescriptorPool(m_device, &poolInfo, nullptr, &m_descriptorPool);

	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create descriptor pool!");
	}
}

void HelloModel::CreateDescriptorSet()
{
	VkDescriptorSetLayout layouts[] = { m_descriptorSetLayout };
	VkDescriptorSetAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = m_descriptorPool;
	allocInfo.descriptorSetCount = 1;
	allocInfo.pSetLayouts = layouts;

	VkResult result = vkAllocateDescriptorSets(m_device, &allocInfo, &m_descriptorSet);

	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("failed to allocate descriptor set!");
	}

	VkDescriptorBufferInfo bufferInfo = {};
	bufferInfo.buffer = m_uniformBuffer;
	bufferInfo.offset = 0;
	bufferInfo.range = sizeof(UniformBufferObject);

	VkDescriptorImageInfo imageInfo = {};
	imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	imageInfo.imageView = m_textureImageView;
	imageInfo.sampler = m_textureSampler;

	std::array<VkWriteDescriptorSet, 2> descriptorWrites = {};
	descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptorWrites[0].dstSet = m_descriptorSet;
	descriptorWrites[0].dstBinding = 0;
	descriptorWrites[0].dstArrayElement = 0;
	descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	descriptorWrites[0].descriptorCount = 1;
	descriptorWrites[0].pBufferInfo = &bufferInfo;

	descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptorWrites[1].dstSet = m_descriptorSet;
	descriptorWrites[1].dstBinding = 1;
	descriptorWrites[1].dstArrayElement = 0;
	descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	descriptorWrites[1].descriptorCount = 1;
	descriptorWrites[1].pImageInfo = &imageInfo;
	
	vkUpdateDescriptorSets(m_device, static_cast<uint32>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
}

void HelloModel::CreateTextureImage()
{
	int texWidth, texHeight, texChannels;

	stbi_uc* pPixels = stbi_load(TEXTURE_PATH.c_str(),
								 &texWidth,
								 &texHeight,
								 &texChannels,
								 STBI_rgb_alpha);

	VkDeviceSize imageSize = texWidth * texHeight * 4;

	if (pPixels == nullptr)
	{
		throw std::runtime_error("failed to load texture image!");
	}

	// host visible memory
	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;

	CreateBuffer(imageSize,
			     VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
				 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
				 stagingBuffer,
				 stagingBufferMemory);

	void* pData;
	vkMapMemory(m_device,
				stagingBufferMemory,
				0,
				imageSize,
				0,
				&pData);

	memcpy(pData, pPixels, static_cast<size_t>(imageSize));

	vkUnmapMemory(m_device, stagingBufferMemory);

	stbi_image_free(pPixels);

	CreateImage(texWidth,
				texHeight,
				VK_FORMAT_R8G8B8A8_UNORM,
				VK_IMAGE_TILING_OPTIMAL,
				VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
				VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
				m_textureImage,
				m_textureImageMemory);

	TransitionImageLayout(
		m_textureImage,
		VK_FORMAT_R8G8B8A8_UNORM,
		VK_IMAGE_LAYOUT_UNDEFINED,
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

	CopyBufferToImage(
		stagingBuffer,
		m_textureImage,
		static_cast<uint32>(texWidth),
		static_cast<uint32>(texHeight));

	TransitionImageLayout(
		m_textureImage,
		VK_FORMAT_R8G8B8_UNORM,
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

	vkDestroyBuffer(m_device, stagingBuffer, nullptr);
	vkFreeMemory(m_device, stagingBufferMemory, nullptr);
}

void HelloModel::CreateImage(
	uint32				  width,
	uint32				  height,
	VkFormat			  format,
	VkImageTiling		  tiling,
	VkImageUsageFlags	  usage,
	VkMemoryPropertyFlags properties,
	VkImage&		      image,
	VkDeviceMemory&		  imageMemory)
{
	VkImageCreateInfo imageInfo = {};
	imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageInfo.imageType = VK_IMAGE_TYPE_2D;					// kind of coordinate system of texels
	imageInfo.extent.width = static_cast<uint32>(width);
	imageInfo.extent.height = static_cast<uint32>(height);
	imageInfo.extent.depth = 1;
	imageInfo.mipLevels = 1;
	imageInfo.arrayLayers = 1;
	imageInfo.format = format; // must be same as pixels in the buffer
	imageInfo.tiling = tiling;
	imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	imageInfo.usage = usage;
	imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	imageInfo.flags = 0;

	VkResult result = vkCreateImage(m_device,
									&imageInfo,
									nullptr,
									&image);

	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create texture image!");
	}

	VkMemoryRequirements memRequirements;
	vkGetImageMemoryRequirements(m_device, image, &memRequirements);

	VkMemoryAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = FindMemoryType(memRequirements.memoryTypeBits,
											   VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	result = vkAllocateMemory(m_device,
							  &allocInfo,
	  						  nullptr,	
	  						  &imageMemory);

	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("failed to allocate memory for image!");
	}

	vkBindImageMemory(m_device, image, imageMemory, 0);
}

VkCommandBuffer HelloModel::BeginSingleTimeCommands()
{
	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = m_commandPool;
	allocInfo.commandBufferCount = 1;

	VkCommandBuffer commandBuffer;
	vkAllocateCommandBuffers(m_device, &allocInfo, &commandBuffer);

	VkCommandBufferBeginInfo beginInfo = {};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	vkBeginCommandBuffer(commandBuffer, &beginInfo);

	return commandBuffer;
}

void HelloModel::EndSingleTimeCommands(VkCommandBuffer commandBuffer) {
	vkEndCommandBuffer(commandBuffer);

	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;

	vkQueueSubmit(m_graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(m_graphicsQueue);

	vkFreeCommandBuffers(m_device, m_commandPool, 1, &commandBuffer);
}

void HelloModel::TransitionImageLayout(
	VkImage image,
	VkFormat format,
	VkImageLayout oldLayout,
	VkImageLayout newLayout)
{
	VkCommandBuffer cmdBuffer = BeginSingleTimeCommands();
	VkPipelineStageFlags sourceStage = 0;
	VkPipelineStageFlags destinationStage = 0;
	VkImageAspectFlags aspectFlags = VK_IMAGE_ASPECT_COLOR_BIT;

	if (newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
		aspectFlags = VK_IMAGE_ASPECT_DEPTH_BIT;

		if (HasStencilComponent(format)) {
			aspectFlags |= VK_IMAGE_ASPECT_STENCIL_BIT;
		}
	}

	VkImageMemoryBarrier barrier = {};
	barrier.sType							= VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.oldLayout						= oldLayout;
	barrier.newLayout						= newLayout;
	barrier.srcQueueFamilyIndex				= VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex				= VK_QUEUE_FAMILY_IGNORED;
	barrier.image							= image;
	barrier.subresourceRange.aspectMask		= aspectFlags;
	barrier.subresourceRange.baseMipLevel	= 0;
	barrier.subresourceRange.levelCount		= 1;
	barrier.subresourceRange.baseArrayLayer = 0;
	barrier.subresourceRange.layerCount		= 1;

	if ((oldLayout == VK_IMAGE_LAYOUT_UNDEFINED) && (newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)) {
		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		sourceStage			  = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		destinationStage	  = VK_PIPELINE_STAGE_TRANSFER_BIT;
	}
	else if ((oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) && (newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)) {
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
		sourceStage			  = VK_PIPELINE_STAGE_TRANSFER_BIT;
		destinationStage	  = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	}
	else if ((oldLayout == VK_IMAGE_LAYOUT_UNDEFINED) && (newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)) {
		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
		sourceStage           = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		destinationStage	  = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	}
	else
	{
		throw std::invalid_argument("unsupported layout transition!");
	}

	vkCmdPipelineBarrier(
		cmdBuffer,
		sourceStage, // pipe stage before
		destinationStage, // pipe stage wait
		0,
		0,
		nullptr,
		0,
		nullptr,
		1,
		&barrier);

	EndSingleTimeCommands(cmdBuffer);
}

void HelloModel::CopyBufferToImage(
	VkBuffer buffer,
	VkImage  image,
	uint32   width,
	uint32   height)
{
	VkCommandBuffer cmdBuffer = BeginSingleTimeCommands();

	VkBufferImageCopy region = {};
	region.bufferOffset      = 0;
	region.bufferRowLength   = 0;
	region.bufferImageHeight = 0;

	region.imageSubresource.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
	region.imageSubresource.mipLevel       = 0;
	region.imageSubresource.baseArrayLayer = 0;
	region.imageSubresource.layerCount	   = 1;

	region.imageOffset = { 0, 0, 0 };
	region.imageExtent =
	{
		width,
		height,
		1
	};

	vkCmdCopyBufferToImage(
		cmdBuffer,
		buffer,
		image,
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		1,
		&region);

	EndSingleTimeCommands(cmdBuffer);
}

void HelloModel::CreateTextureImageView()
{
	m_textureImageView = CreateImageView(m_textureImage, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT);
}

VkImageView HelloModel::CreateImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags)
{
	VkImageViewCreateInfo viewInfo = {};
	viewInfo.sType							 = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	viewInfo.image							 = image;
	viewInfo.viewType						 = VK_IMAGE_VIEW_TYPE_2D;
	viewInfo.format							 = format;
	viewInfo.subresourceRange.aspectMask     = aspectFlags;
	viewInfo.subresourceRange.baseMipLevel   = 0;
	viewInfo.subresourceRange.levelCount     = 1;
	viewInfo.subresourceRange.baseArrayLayer = 0;
	viewInfo.subresourceRange.layerCount	 = 1;

	VkImageView imageView;
	VkResult result = vkCreateImageView(m_device, &viewInfo, nullptr, &imageView);

	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create texture image view!");
	}
	
	return imageView;
}

void HelloModel::CreateTextureSampler()
{
	VkSamplerCreateInfo samplerInfo = {};
	samplerInfo.sType				    = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	samplerInfo.magFilter				= VK_FILTER_LINEAR;
	samplerInfo.minFilter				= VK_FILTER_LINEAR;
	samplerInfo.addressModeU			= VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeV			= VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeW			= VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.anisotropyEnable		= VK_TRUE;
	samplerInfo.maxAnisotropy			= 16;
	samplerInfo.borderColor				= VK_BORDER_COLOR_INT_OPAQUE_BLACK;
	samplerInfo.unnormalizedCoordinates = VK_FALSE; // which addressing mode to use
	samplerInfo.compareEnable			= VK_FALSE;
	samplerInfo.compareOp				= VK_COMPARE_OP_ALWAYS;
	samplerInfo.mipmapMode				= VK_SAMPLER_MIPMAP_MODE_LINEAR;
	samplerInfo.mipLodBias				= 0.0f;
	samplerInfo.minLod				    = 0.0f;
	samplerInfo.maxLod					= 0.0f;

	VkResult result = vkCreateSampler(m_device, &samplerInfo, nullptr, &m_textureSampler);

	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create texture sampler!");
	}
}

void HelloModel::CreateDepthResources()
{
	VkFormat depthFormat = FindDepthFormat();
	CreateImage(m_swapchainExtent.width, m_swapchainExtent.height, depthFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
		        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_depthImage, m_depthImageMemory);
	m_depthImageView = CreateImageView(m_depthImage, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT);
	TransitionImageLayout(m_depthImage, depthFormat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
}
	
VkFormat HelloModel::FindSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features)
{
	for (VkFormat format : candidates)
	{
		VkFormatProperties props;
		vkGetPhysicalDeviceFormatProperties(m_physicalDevice, format, &props);

		if ((tiling == VK_IMAGE_TILING_LINEAR) && (props.linearTilingFeatures & features) == features) {
			return format;
		}
		else if ((tiling == VK_IMAGE_TILING_OPTIMAL) && (props.optimalTilingFeatures & features) == features) {
			return format;
		}
	}

	assert(false);
	throw std::runtime_error("failed to find supported format!");
}


VkFormat HelloModel::FindDepthFormat()
{
	return FindSupportedFormat(
		{ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
		VK_IMAGE_TILING_OPTIMAL,
		VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
}

bool HelloModel::HasStencilComponent(VkFormat format)
{
	return (format == VK_FORMAT_D32_SFLOAT_S8_UINT) ||
		   (format == VK_FORMAT_D24_UNORM_S8_UINT);
}

void HelloModel::LoadModel()
{
	std::cout << "Loading Model..." << std::endl;	
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string err;

	bool result = tinyobj::LoadObj(&attrib, &shapes, &materials, &err, MODEL_PATH.c_str());
	std::cout << err << std::endl;

	assert(result != false);
	if (result == false)
	{
		throw std::runtime_error(err);
	}

	for (const auto& shape : shapes){
		for (const auto& index : shape.mesh.indices) {
			Vertex vertex = {};
			vertex.pos = {
				attrib.vertices[3 * index.vertex_index + 0],
				attrib.vertices[3 * index.vertex_index + 1],
				attrib.vertices[3 * index.vertex_index + 2]
			};
			vertex.texCoord = {
				attrib.texcoords[2 * index.texcoord_index + 0],
				1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
			};
			m_vertices.push_back(vertex);
			m_indices.push_back(m_indices.size());			
		}
	}
	std::cout << "Done loading Model." << std::endl;
}
