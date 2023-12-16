cmake_minimum_required(VERSION 3.4...3.20 FATAL_ERROR)
project(3DEngine)

set(CMAKE_CXX_STANDARD 23)

# Vulkan path to SDK
set(ENV{VULKAN_SDK} "C:/VulkanSDK/1.3.268.0")
find_package(Vulkan REQUIRED)

add_executable(${PROJECT_NAME} main.cpp
        Engine/Applications/RemApplication.cpp
        Engine/Applications/RemApplication.h
        "Engine/Graphics/API/Vulkan/RemPipeline.cpp"
        "Engine/Graphics/API/Vulkan/RemPipeline.h"
        "Engine/Graphics/API/Vulkan/SystemStructs.h"
        "Engine/Graphics/API/Vulkan/Wrappers/VulkanMaterial.cpp"
        "Engine/Graphics/API/Vulkan/Wrappers/VulkanMaterial.h"
        "Engine/IO/File Management/FileManagement.cpp"
        "Engine/IO/File Management/FileManagement.h"
        "Engine/Graphics/API/Vulkan/RemSwapChain.cpp"
        "Engine/Graphics/API/Vulkan/RemSwapChain.h"
        "Engine/IO/File Management/FileManagementInfo.h"
        "Engine/Graphics/API/Vulkan/Wrappers/Data/Vertex.h"
        "Engine/Graphics/API/Vulkan/Wrappers/RemRenderer.cpp"
        "Engine/Graphics/API/Vulkan/Wrappers/RemRenderer.h"
        Engine/Graphics/API/Base/Wrappers/Material.h)

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

add_subdirectory(glfw)
target_link_libraries(${PROJECT_NAME} glfw)
include_directories("glfw/include")

# Link GLM
add_subdirectory(glm)

# Link libraries
target_link_libraries(
        ${PROJECT_NAME}
        Vulkan::Vulkan
        glm::glm)

