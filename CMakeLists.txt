cmake_minimum_required(VERSION 3.4...3.20 FATAL_ERROR)
project(3DEngine)

set(CMAKE_CXX_STANDARD 23)

# Vulkan path to SDK
set(ENV{VULKAN_SDK} "C:/VulkanSDK/1.3.268.0")
find_package(Vulkan REQUIRED)

add_executable(${PROJECT_NAME} main.cpp Project/Applications/RemApplication.cpp Project/Applications/RemApplication.h Project/Core/Systems/RemPipeline.cpp Project/Core/Systems/RemPipeline.h Project/Core/Systems/SystemStructs.h Project/Core/Systems/RemShaderComponent.cpp Project/Core/Systems/RemShaderComponent.h "Project/Core/Helpers/File Management/RemFileManagement.cpp" "Project/Core/Helpers/File Management/RemFileManagement.h" Project/Core/Systems/RemSwapChain.cpp Project/Core/Systems/RemSwapChain.h)

# Include Vulkan Headers
include_directories("${VULKAN_SDK}/Include")
include_directories(Project/Core)
include_directories(Project/Core/Systems)
include_directories(Project/Applications)

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

