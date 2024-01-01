cmake_minimum_required(VERSION 3.4...3.20 FATAL_ERROR)
project(3DEngine)

set(CMAKE_CXX_STANDARD 23)

file(GLOB_RECURSE PROJECT_SOURCE_FILES "Engine/*.cpp")
file(GLOB_RECURSE PROJECT_HEADER_FILES "Engine/*.h")

add_executable(${PROJECT_NAME}
        main.cpp
        ${PROJECT_HEADER_FILES}
        ${PROJECT_SOURCE_FILES})

# Shorten some common include directories
include_directories(Engine/Graphics/API)
include_directories(Engine/Applications)
include_directories(Engine/IO)
include_directories(Engine/Common)
include_directories(Engine/System)


# Link Dependencies
add_subdirectory(Libraries)

# Make assets
add_subdirectory(Assets)


