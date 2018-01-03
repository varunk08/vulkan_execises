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


### image Views  
an image view is literally a view into an image.  
an image view is sufficient to start using an image as a texture but it is not ready to be used as a render target. that requires one more step of indirection, known as a framebuffer.  

### graphics pipeline
stages:  
Input assembler,  
Vertex shader,  
tessellation shader,  
geometry shader,  
rasterization,  
fragment shader,  
color blending stage.  


### shaders  
shader code in vulkan has to be specified in bytecode format.  
this format is called `SPIR-v`  
both vulkan and openCL can use it  
advantages of bytecode format: compilers by GPU vendors to turn shader code (glsl, hlsl) into native code (ISA) is less complex.  
khronos has released vendor independent compiler that compiles GLSL to SPIR-v  
it is included with the LunarG SDK as `glslangValidator.exe`  

GLSL uses global variables to handle input and output  
vector type is `vec`  

clip coordinate is a four dimensional vector from the vertex shader that is subsequently turned into a normalized device coordinate by dividing the whole vector by its last component.  
The normalized device coordinates are homogenous coordinates that map the framebuffer to a [-1, 1] by [-1, 1] coordinate system  
The sign of the Y coordinate is flipped compared to OpenGL (+ve is down)  

"creating a vertex buffer in vulkan and filling it with data is not trivial!"  
The `GL_ARB_separate_shader_objects` extension is required for vulkan shaders to work.  

`layout(location=0)` modifier specifies the index of the framebuffer.  

Before passing in the bytecode to the pipeline we have to wrap it in a `VkShaderModule`  

### fixed function state settings  
vertex input state `VkPipelineVertexInputStateCreateInfo`   
input assembly state `VkPipelineInputAssemblyStateCreateInfo`   

view port and scissor  
viewport describes the region of the framebuffer that the output will be rendered to.  
they define the transformation from the image to the framebuffer.  
combine viewport and scissor state into: `VkPipelineViewportStateCreateInfo`  

Rasterizer `VkPipelineRasterizationStateCreateInfo `  
Multisampling: `VkPipelineMultisampleStateCreateInfo `  
Depth and stencil testing: `VkPipelineDepthStencilStateCreateInfo`  
Color blending: blending the fragment generated with what is already in the framebuffer. `VkPipelineColorBlendAttachmentState` configures per attached framebuffer and `VkPipelineColorBlendStateCreateInfo` configures global color blending settings.  

Dynamic state: ` VkPipelineDynamicStateCreateInfo` is used to configure a limited amount of state without recreating the pipeline  
dynamic state can be changed during draw time  


`uniform` values are globals similar to dynamic state variables that can be changed at drawing time to alter behaviour of shaders without having to recreate them.  
used commonly to pass transformation matrix or texture samplers in fragment shader.  
these uniform variables need to be specified during pipeline creation by creating a `VkPipelineLayout`  

### framebuffers
attachments specified during render pass creation are bound by wrapping them into a `VkFramebuffer` object.  
