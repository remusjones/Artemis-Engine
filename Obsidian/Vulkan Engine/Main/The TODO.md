### Development Steps
1. Use https://vulkan-tutorial.com/ to create the fundementals of the Engine Rendering.
2. Extrapolate the "Base-Line" fundamentals of the Engine into seperate classes.
	1. RenderPipeline class for Shader Creation, textures, frag/verts, buffers, etc. 
	2. Material class to contain the Textures, Frags, Verts, Buffers, etc.
	3. A Model class to contain the Material Class and drawing information 
	4. A GameObject class to pivot/move the Models and Materials around. 



### Current Steps: 
1. Implement Shader Modules:
https://vulkan-tutorial.com/Drawing_a_triangle/Graphics_pipeline_basics/Shader_modules
2. Implement Fixed Functions https://vulkan-tutorial.com/Drawing_a_triangle/Graphics_pipeline_basics/Fixed_functions
3. Triangle Displaying, though a validation error needs to be fixed
![[Pasted image 20211107150328.png]]
```cpp
validation layer: Validation Error: [ VUID-vkQueueSubmit-pCommandBuffers-00071 ] Object 0: handle = 0x2bfcf8b2020, type
= VK_OBJECT_TYPE_DEVICE; | MessageID = 0x2e2f4d65 | vkQueueSubmit(): pSubmits[0].pCommandBuffers[0] VkCommandBuffer 0x2b
fd4401870[] is already in use and is not marked for simultaneous use. The Vulkan spec states: If any element of the pCom
mandBuffers member of any element of pSubmits was not recorded with the VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT, it
 must not be in the pending state 
```

https://vulkan.lunarg.com/doc/view/1.2.189.2/windows/1.2-extensions/vkspec.html#VUID-vkQueueSubmit-pCommandBuffers-00071

https://vulkan-tutorial.com/Drawing_a_triangle/Drawing/f

4. Implement Vertex Buffers
	https://vulkan-tutorial.com/Vertex_buffers/Vertex_input_description