# TODO: Move these into Libraries CMake
# Find Vulkan
find_package(Vulkan REQUIRED)
include_directories("$ENV{VULKAN_SDK}/Include")

# https://github.com/google/glog.git
message("Adding glog")
set(WITH_GFLAGS OFF) # We aren't using gflags, so disable it
add_subdirectory(glog glog)

# https://github.com/g-truc/glm.git
message("Adding glm")
add_subdirectory(glm glm)

# https://github.com/libsdl-org/SDL.git
message("Adding sdl")
set(SDL_DIRECTX OFF) # Disable due to not supporting directx at the moment
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
target_link_libraries(imgui SDL3::SDL3)

# https://github.com/nothings/stb.git
message("Adding stb")
set(STB_SOURCES "stb/stb_image.h")
target_include_directories(${PROJECT_NAME} PRIVATE "stb/")

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

