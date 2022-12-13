# VulkanProject

## Setup using CLion (Windows)


1. Install Vulkan SDK https://www.lunarg.com/vulkan-sdk/
2. Modify CMakeLists.txt to reflect install directory:

```cpp 
include_directories("C:/VulkanSDK/1.3.231.1/Include")
target_link_libraries(3DEngine glfw C:/VulkanSDK/1.3.231.1/Lib/vulkan-1.lib glm)
```


3. git clone https://github.com/glfw/glfw.git
4. git clone https://github.com/g-truc/glm.git

## Install MinGW Toolchain for CLion

1. Install MSys2 ([http://www.msys2.org/])
2. Install a toolchain: pacman -S mingw-w64-x86_64-toolchain (inside of the msys2.exe console)
3. Select the mingw installation in CLion:
4. File | Settings | Build ,Execution... | Toolchains
5. Set MinGW Home to {MSys2 installation dir}\mingw64

## Preparing Shader Compilation
1. Navigate to the %repo%/Automation/CompileShaders.bat
2. Make sure the path is set to your vulkan directory. 
3. Navigate to Settings > External Tools and add the CompileShaders.bat to your tools list.
4. Right Click -> External Tools > CompileShaders.

Note: your shader vert/frags should have a unique name, otherwise they will be overwritten.
