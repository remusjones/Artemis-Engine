cmake_minimum_required(VERSION 3.4...3.20 FATAL_ERROR)
project(3DEngine)

set(CMAKE_CXX_STANDARD 23)

# Path to Libs
set(LIBRARIES_PATH "Libraries/")

add_executable(${PROJECT_NAME} main.cpp
        Engine/Applications/VulkanGraphicsImpl.cpp
        Engine/Applications/VulkanGraphicsImpl.h
        "Engine/Graphics/API/Vulkan/VulkanEngine.cpp"
        "Engine/Graphics/API/Vulkan/VulkanEngine.h"
        "Engine/Graphics/API/Vulkan/VulkanSystemStructs.h"
        "Engine/IO/File Management/FileManagement.cpp"
        "Engine/IO/File Management/FileManagement.h"
        "Engine/Graphics/API/Vulkan/VulkanSwapChain.cpp"
        "Engine/Graphics/API/Vulkan/VulkanSwapChain.h"
        "Engine/IO/File Management/FileManagementInfo.h"
        "Engine/Graphics/API/Base/Common/Data/Vertex.h"
        Engine/Graphics/API/Base/Common/MaterialBase.h
        Engine/Graphics/API/Base/Common/RendererBase.h
        Engine/Applications/IApplication.h
        Engine/Graphics/API/Base/Common/Material.h
        Engine/Graphics/API/Base/Common/Material.cpp
        Engine/Graphics/API/Base/Common/Material.h
        Engine/Graphics/API/Vulkan/Common/MeshObject.cpp
        Engine/Graphics/API/Vulkan/Common/MeshObject.h
        Engine/Graphics/API/Base/Common/MaterialBase.cpp
        Engine/Graphics/API/Base/Common/MaterialBase.h
        Engine/Graphics/API/Vulkan/GraphicsPipeline.cpp
        Engine/Graphics/API/Vulkan/GraphicsPipeline.h
        Engine/Graphics/API/Base/Common/Buffers/PushConstants.h
        Engine/Graphics/API/Base/Common/Buffers/AllocatedVertexBuffer.h
        Engine/Graphics/API/Base/Common/Buffers/AllocatedVertexBuffer.cpp
        Engine/Graphics/API/Base/Common/Buffers/AllocatedVertexBuffer.cpp
        Engine/Graphics/API/Base/Common/Buffers/AllocatedVertexBuffer.h
        Engine/Graphics/API/Base/Common/Buffers/AllocatedBuffer.cpp
        Engine/Graphics/API/Base/Common/Buffers/AllocatedBuffer.h
        Engine/Graphics/API/Base/Common/Data/Mesh.cpp
        Engine/Graphics/API/Base/Common/Data/Mesh.h
        Engine/Graphics/API/Base/Common/Buffers/AllocatedImage.h
        Engine/Graphics/API/Vulkan/Helpers/VulkanInitialization.h
        Engine/Common/Scenes/Scene.cpp
        Engine/Common/Scenes/Scene.h
        Engine/Common/Objects/Camera.cpp
        Engine/Common/Objects/Camera.h
        Engine/Common/Math/Transform.cpp
        Engine/Common/Math/Transform.h
        Engine/Common/Scenes/SandboxScene.cpp
        Engine/Common/Scenes/SandboxScene.h
        Engine/Common/Objects/Super.cpp
        Engine/Common/Objects/Super.h
        Engine/Graphics/API/Base/Common/Data/GPUCameraData.cpp
        Engine/Graphics/API/Base/Common/Data/GPUCameraData.h
        Engine/Graphics/API/Vulkan/Helpers/BindingsHelper.h
        Engine/Graphics/API/Base/Common/Data/FrameData.h
        Engine/Graphics/API/Base/Common/Data/GPUSceneData.h
        Engine/Common/Objects/ImGuiLayer.cpp
        Engine/Common/Objects/ImGuiLayer.h
        Engine/Graphics/API/Base/Common/Image.h
        Engine/Graphics/API/Base/Common/Image.cpp
        Engine/IO/LoadUtilities.cpp
        Engine/IO/LoadUtilities.h
        Engine/Graphics/API/Base/Common/Buffers/Texture.h
        Engine/Graphics/API/Base/Common/Buffers/Texture.cpp)

# Shorten some common include directories
include_directories(Engine/Graphics/API)
include_directories(Engine/Applications)
include_directories(Engine/IO)
include_directories(Engine/Common)

# Find Vulkan
find_package(Vulkan REQUIRED)
include_directories("$ENV{VULKAN_SDK}/Include")

add_subdirectory(Libraries)
# Compile SPIR-V Shaders
add_subdirectory(Shaders)

# Link libraries
target_link_libraries(
        ${PROJECT_NAME}
        Vulkan::Vulkan
        glm::glm
        glog
        SDL3::SDL3
        GPUOpen::VulkanMemoryAllocator
        tinyobjloader
        imgui)

