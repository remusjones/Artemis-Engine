cmake_minimum_required(VERSION 3.20)
project(3DEngine)


set(CMAKE_CXX_STANDARD 23)

add_executable(3DEngine main.cpp Project/Applications/rem_window.cpp Project/Applications/rem_window.h Project/Core/Systems/rem_pipeline.cpp Project/Core/Systems/rem_pipeline.h Project/Core/Systems/SystemStructs.h Project/Core/Systems/rem_shaderComponent.cpp Project/Core/Systems/rem_shaderComponent.h "Project/Core/Helpers/File Management/rem_fileManagement.cpp" "Project/Core/Helpers/File Management/rem_fileManagement.h")

# Include Vulkan Headers
include_directories("C:/VulkanSDK/1.2.189.2/Include")
include_directories(Project/Core)
include_directories(Project/Core/Systems)
include_directories(Project/Applications)


add_library(glm INTERFACE)
target_include_directories(glm INTERFACE "glm")

add_subdirectory("glfw" glfw)

target_link_libraries(3DEngine glfw C:/VulkanSDK/1.2.189.2/Lib/vulkan-1.lib glm)

