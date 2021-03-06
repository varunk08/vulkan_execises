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

### fences and semaphores
the operations in the swap chain are not guaranteed to be finished in order. but the execution in the order specified is necessary   
fences or semaphores can be used to synchronize the order of operations within the swapchain  
state of fences can be accessed from your program and semaphores cannot be  
fences are designed to synchronize your application itself with rendering operation  
semaphores are used to synchronize operations within or across command queues  

### vertex data  
binding descriptions: passing the vertex information to the vertex shader once it's uploaded to gpu memory  
vertex binding describes at which rate to load data from memory throughout the vertices. it specifies the number of bytes between data entries and whether to move to the next data entry after each vertex or after each instance.  
the second structure that describes how to handle vertex input is `VkVertexInputAttributeDescription`  
the format parameter describes the type of data. formats are specified using the same enumeration as color formats  


### Resource descriptors  
a descriptor is a way for shaders to freely access resources like buffers and images.  
usage of descriptors consists of three parts:  
1. specify a descriptor layout during pipeline creation.  
2. allocate a descriptor set from a descriptor pool.  
3. bind the descriptor set during rendering.  

descriptor layout specifies the types of resources that are going to be accessed by the pipeline.  
push constants are more efficient than uniform buffer objects for frequently changing data.  

### Texture mapping  
using a texture needs the following steps:  
create an image object backed by device memory  
fill with pixels from an image file  
create an image sampler  
add a combined image sampler descriptor to sample colors from the texture 

images can have different layouts that affect how the pixels are organized in memory. storing the pixels row by row may not lead to the best performance

some layouts and what they are optimal for:  
`VK_IMAGE_LAYOUT_PRESENT_SRC_KHR` optimal for presentation  
`VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL`: optimal as attachment for writing colors from the fragment shader  
`VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL`: source in a transfer operation  
`VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL`: destination of transfer operation  
`VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL`: sampling from a shader  

__pippeline barriers__ are used to transition the layout of an image and synchronize access to resources (making sure the write to an image is finished before read) and also for transferring queue family ownership when using `VK_SHARING_MODE_EXCLUSIVE`.  

`STB_IMAGE_IMPLEMENTATION` must be defined in a .c or .cpp and not a header file. This is because when that is defined, the stb functions are defined. So having it in the c or cpp files makes sure it is defined only once.  

image object make it easier and faster to retrieve colors by allowing us to use 2D coordinates.
tiling mode cannot be changed at a later time.  
undefined layout: not usable by Gpu, but very first layout transition will discard the texels.  

### Samplers
it's possible for shaders to read texels directly from images.  
but this is not how it's done. textures are read through samplers. samplers will apply filtering and transformations to compute the final color that is retrieved  
reading outside the image through it's addressing mode: repeat, mirrored repeat, clamp to edge, clamp to border.  

`magFilter` and `minFilter` fields specify how to interpolate texels that are magnified or minified.  
Magnification concerns the oversampling problem described above and minification concerns undersampling.  

descriptor for images: combined image sampler  


### Depth buffering
depth image should have the same resolution as the color attachment defined by the swap chain extent.
create depth image, image view, image memory.  
find supported formats  
enable depth bounds testing in device features   
set pipeline barrier layout transitions  
update depth attachment to render pass  
update pipeline state with depth stencil info  
recreate depth buffer with swapchain extent for handling resize  


### Loading 3D models  


