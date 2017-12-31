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
