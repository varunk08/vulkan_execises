### Draw triangle  

First vulkan triangle!

### validation layers:  
Validation layers can be used if they have been onto the system. For example, the LunarG validation layers are only available on PCs with the Vulkan SDK installed.  
The SDK allows you to request the `VK_LAYER_LUNARG_standard_validation` layer that implicitly enables a whole range of useful diagnostics layers.  
The `NDEBUG` macro is part of the C++ standard and means not debug.  


### physical device
`vkGetPhysicalDeviceProperties` gives basic device properties.  
`vkGetPhysicalDeviceFeatures` gives info about support for optional features like texture compression, 64bit floats and multi-viewport rendering.  

### queue families
`vkGetPhysicalDeviceQueueFamilyProperties` is used to retrieve a list of queue families.  

### logical device
After creating a physical device, we need to create a `logical device` to interface with it.  
Multiple logical devices can be created from the same physical device.  

### window surface  
`VK_KHR_surface` is the WSI extension used to present to the Screen. It exposes `VkSurfaceKHR` object that represents an abstract type of surface to present rendered images to.   
window surfaces are entirely optional, vulkan being platform agnostic.  
The window surface needs to be created right after the instance creation, because it can actually  influence the  physical device selection.  
Creation of this `VkSurfaceKHR` is platform dependent though, so there is a platform specific addition to the extension, which for windows is `VK_KHR_win32_surface`.  


### swap Chain  
essentially a queue of images waiting to be presented on the screen.  
synchronizes the presentation of images with the refresh rate of the screen.  
`VK_KHR_swapchain` extension must be enabled  
Creating a swap chain also involves a lot more settings than instance and device creation. we need to query support for surface capabilities, surface formats, available presentation modes.  
Three types of settings must be determined for the best possible swap chain:  
surface format (color depth), presentation mode (conditions for "swapping" images to the screen), swap extent (resolution of images in swap chain)  
Each `VkSurfaceFormatKHR` contains a format and a color space.  
SRGB color space results in more accurate perceived colors.  
If the surface has no preferred format, vulkan indicates this by returning one `VkSurfaceFormatKHR` entry which has its format member set to `VK_FORMAT_UNDEFINED`.  
Presentation modes:  
`VK_PRESENT_MODE_IMMEDIATE_KHR`: transferred to the screen right away  
`VK_PRESENT_MODE_FIFO_KHR`: swap chain is a queue. if queue is full program has to wait. similar to vsync.  
`VK_PRESENT_MODE_FIFO_RELAXED_KHR`: if the queue is empty, instead of waiting for the next vertical blank, the image is transferred to the screen right away.  
`VK_PRESENT_MODE_MAILBOX_KHR`: the images in the queue are replaced with the newer ones if the queue is full.  
Only `VK_PRESENT_MODE_FIFO_KHR` is guaranteed to be available.  


Vulkan tells us to match the resolution of the window by setting the width and height in the `currentExtent`. Some window managers do allow us to differ here and this is indicated by setting the width and height in `currentExtent` to a special value: the maximum value of `uint32_t`. 
