# VulkanProject

## Setup using CLion 


1. Install Vulkan SDK https://www.lunarg.com/vulkan-sdk/
2. Modify CMakeLists.txt to reflect install directory:

```cpp 
include_directories("C:/VulkanSDK/1.2.189.2/Include")
target_link_libraries(3DEngine glfw C:/VulkanSDK/1.2.189.2/Lib/vulkan-1.lib glm)
```


3. git clone https://github.com/glfw/glfw.git
4. git clone https://github.com/g-truc/glm.git

## Install MinGW Toolchain for CLion

1. Install MSys2 ([http://www.msys2.org/])
2. Install a toolchain: pacman -S mingw-w64-x86_64-toolchain
3. Select the mingw installation in CLion:
4. File | Settings | Build ,Execution... | Toolchains
5. Set MinGW Home to {MSys2 installation dir}\mingw64
