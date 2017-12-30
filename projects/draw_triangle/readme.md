### Draw triangle  

First vulkan triangle!

### Validation layers:  
Validation layers can be used if they have been onto the system. For example, the LunarG validation layers are only available on PCs with the Vulkan SDK installed.  
The SDK allows you to request the `VK_LAYER_LUNARG_standard_validation` layer that implicitly enables a whole range of useful diagnostics layers.  
The `NDEBUG` macro is part of the C++ standard and means not debug.  


### Physical device
`vkGetPhysicalDeviceProperties` gives basic device properties.  
`vkGetPhysicalDeviceFeatures` gives info about support for optional features like texture compression, 64bit floats and multi-viewport rendering.  

### Queue families
`vkGetPhysicalDeviceQueueFamilyProperties` is used to retrieve a list of queue families.  
