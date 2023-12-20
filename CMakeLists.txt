cmake_minimum_required(VERSION 3.4...3.20 FATAL_ERROR)
project(3DEngine)

set(CMAKE_CXX_STANDARD 23)

# Vulkan path to SDK
set(ENV{VULKAN_SDK} "C:/VulkanSDK/1.3.268.0")
set(LIBRARIES_PATH "Libraries/")


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
        Engine/Graphics/API/Base/Common/RendererBase.h
        Engine/Applications/IApplication.h
        Engine/Graphics/API/Base/Common/Material.h
        Engine/Graphics/API/Base/Common/Buffers/Buffer.h
        Engine/Graphics/API/Base/Common/Buffers/Buffer.cpp
        Engine/Graphics/API/Base/Common/Buffers/Buffer.h
        Engine/Graphics/API/Base/Common/Material.cpp
        Engine/Graphics/API/Base/Common/Material.h
        Engine/Graphics/API/Vulkan/Common/MeshObject.cpp
        Engine/Graphics/API/Vulkan/Common/MeshObject.h
        Engine/Graphics/API/Base/Common/MaterialBase.cpp
        Engine/Graphics/API/Base/Common/MaterialBase.h
        Engine/Graphics/API/Vulkan/GraphicsPipeline.cpp
        Engine/Graphics/API/Vulkan/GraphicsPipeline.h
        Engine/Graphics/API/Base/Common/Buffers/PushConstants.h
        Engine/Graphics/API/Base/Common/Buffers/VertexBuffer.cpp
        Engine/Graphics/API/Base/Common/Buffers/VertexBuffer.h
        Engine/Graphics/API/Base/Common/Buffers/AllocatedVertexBuffer.h
        Engine/Graphics/API/Base/Common/Buffers/AllocatedVertexBuffer.cpp
        Engine/Graphics/API/Base/Common/Buffers/AllocatedVertexBuffer.cpp
        Engine/Graphics/API/Base/Common/Buffers/AllocatedVertexBuffer.h
        Engine/Graphics/API/Base/Common/Buffers/AllocatedBuffer.cpp
        Engine/Graphics/API/Base/Common/Buffers/AllocatedBuffer.h
        Engine/Graphics/API/Base/Common/Data/Mesh.cpp
        Engine/Graphics/API/Base/Common/Data/Mesh.h
        Engine/Graphics/API/Base/Common/AllocatedImage.h)

# Include Vulkan Headers
include_directories("${VULKAN_SDK}/Include")

include_directories(Engine/Graphics/API)
include_directories(Engine/Applications)
include_directories(Engine/IO)

# https://github.com/google/glog.git
set(WITH_GFLAGS OFF) # We aren't using gflags, so disable it
add_subdirectory(${LIBRARIES_PATH}glog glog)

# https://github.com/g-truc/glm.git
add_subdirectory(${LIBRARIES_PATH}glm glm)

# https://github.com/libsdl-org/SDL.git
set(SDL_DIRECTX OFF) # Disable due to not supporting directx at the moment
add_subdirectory(${LIBRARIES_PATH}SDL EXCLUDE_FROM_ALL)

# https://github.com/charles-lunarg/vk-bootstrap.git
add_subdirectory(${LIBRARIES_PATH}VulkanMemoryAllocator VulkanMemoryAllocator)

# https://github.com/tinyobjloader/tinyobjloader.git
add_subdirectory(${LIBRARIES_PATH}tinyobjloader tinyobjloader)

# Compile SPV Shaders TODO: Move to Vulkan only CMake script
add_custom_target(CompileSPVShaders
        COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/CompileShaders.bat
        COMMENT "Compiling SPV Shaders"
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        )

add_dependencies(${PROJECT_NAME} CompileSPVShaders)
# Link libraries
target_link_libraries(${PROJECT_NAME} )
target_link_libraries(
        ${PROJECT_NAME}
        Vulkan::Vulkan
        glm::glm
        glog
        SDL3::SDL3
        GPUOpen::VulkanMemoryAllocator
        tinyobjloader)

