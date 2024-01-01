# Find Vulkan
find_package(Vulkan REQUIRED)
include_directories("$ENV{VULKAN_SDK}/Include")

# https://github.com/gabime/spdlog.git
message("Adding spdlog")
add_subdirectory(spdlog)
include_directories(spdlog/include)

# https://github.com/g-truc/glm.git
message("Adding glm")
add_subdirectory(glm glm)

# https://github.com/libsdl-org/SDL.git
message("Adding sdl")
set(SDL_STATIC TRUE)
add_subdirectory(SDL)

# https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator.git
message("Adding VulkanMemoryAllocator")
add_subdirectory(VulkanMemoryAllocator VulkanMemoryAllocator)

# https://github.com/tinyobjloader/tinyobjloader.git
message("Adding tinyobjloader")
add_subdirectory(tinyobjloader tinyobjloader)

# https://github.com/ocornut/imgui.git
message("Adding ImGui")
set(IMGUI_PATH imgui)
add_library(imgui STATIC ${IMGUI_PATH})
target_include_directories(imgui PUBLIC ${IMGUI_PATH} ${IMGUI_PATH}/backends)
target_sources(imgui PUBLIC
        "${IMGUI_PATH}/imgui.h"
        "${IMGUI_PATH}/imgui.cpp"
        "${IMGUI_PATH}/imgui_demo.cpp"
        "${IMGUI_PATH}/imgui_draw.cpp"
        "${IMGUI_PATH}/imgui_widgets.cpp"
        "${IMGUI_PATH}/imgui_tables.cpp"
        "${IMGUI_PATH}/backends/imgui_impl_vulkan.cpp"
        "${IMGUI_PATH}/backends/imgui_impl_sdl3.cpp"
)
target_link_libraries(imgui SDL3::SDL3-static)

# https://github.com/nothings/stb.git
message("Adding stb")
set(STB_SOURCES "stb/stb_image.h")
target_include_directories(${PROJECT_NAME} PRIVATE "stb/")

# Link libraries
target_link_libraries(
        ${PROJECT_NAME}
        Vulkan::Vulkan
        glm::glm
        spdlog::spdlog
        SDL3::SDL3-static
        GPUOpen::VulkanMemoryAllocator
        tinyobjloader
        imgui)

