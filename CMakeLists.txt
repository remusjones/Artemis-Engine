cmake_minimum_required(VERSION 3.4...3.20 FATAL_ERROR)
project(3DEngine)

set(CMAKE_CXX_STANDARD 23)

# Vulkan path to SDK
set(ENV{VULKAN_SDK} "C:/VulkanSDK/1.3.268.0")
find_package(Vulkan REQUIRED)

add_executable(${PROJECT_NAME} main.cpp
        Engine/Applications/VulkanGraphicsImpl.cpp
        Engine/Applications/VulkanGraphicsImpl.h
        "Engine/Graphics/API/Vulkan/VulkanPipelineManager.cpp"
        "Engine/Graphics/API/Vulkan/VulkanPipelineManager.h"
        "Engine/Graphics/API/Vulkan/VulkanSystemStructs.h"
        "Engine/Graphics/API/Vulkan/Common/VulkanMaterial.cpp"
        "Engine/Graphics/API/Vulkan/Common/VulkanMaterial.h"
        "Engine/IO/File Management/FileManagement.cpp"
        "Engine/IO/File Management/FileManagement.h"
        "Engine/Graphics/API/Vulkan/VulkanSwapChain.cpp"
        "Engine/Graphics/API/Vulkan/VulkanSwapChain.h"
        "Engine/IO/File Management/FileManagementInfo.h"
        "Engine/Graphics/API/Base/Common/Data/Vertex.h"
        Engine/Graphics/API/Base/Common/MaterialBase.h
        Engine/Graphics/API/Base/Common/RendererBase.h Engine/Applications/IApplication.h Engine/Graphics/API/Base/Common/Material.h Engine/Graphics/API/Base/Common/Buffer.h Engine/Graphics/API/Base/Common/Buffer.cpp Engine/Graphics/API/Base/Common/Buffer.h Engine/Graphics/API/Base/Common/Material.cpp Engine/Graphics/API/Base/Common/Material.h Engine/Graphics/API/Vulkan/Common/TriangleObject.cpp Engine/Graphics/API/Vulkan/Common/TriangleObject.h Engine/Graphics/API/Base/Common/MaterialBase.cpp Engine/Graphics/API/Base/Common/MaterialBase.h Engine/Graphics/API/Vulkan/GraphicsPipeline.cpp Engine/Graphics/API/Vulkan/GraphicsPipeline.h)

# Include Vulkan Headers
include_directories("${VULKAN_SDK}/Include")

include_directories(Engine/Graphics)
include_directories(Engine/Graphics/API/Vulkan API)
include_directories(Engine/Applications)
include_directories(Engine/IO/File Management)

# Configure and link GLFW
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

# https://github.com/glfw/glfw.git
add_subdirectory(glfw)
target_link_libraries(${PROJECT_NAME} glfw)
include_directories("glfw/include")

# https://github.com/google/glog.git
add_subdirectory(glog)

# https://github.com/g-truc/glm.git
add_subdirectory(glm)

# Link libraries
target_link_libraries(
        ${PROJECT_NAME}
        Vulkan::Vulkan
        glm::glm glog)

