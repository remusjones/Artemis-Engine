cmake_minimum_required(VERSION 3.4...3.20 FATAL_ERROR)
project(3DEngine)
set(CXX_STANDARD_REQUIRED ON)
#set(CMAKE_CXX_STANDARD 23) #Issues with static compiles with 23 selected

file(GLOB_RECURSE PROJECT_SOURCE_FILES "Engine/*.cpp")
file(GLOB_RECURSE PROJECT_HEADER_FILES "Engine/*.h")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${GCC_COVERAGE_LINK_FLAGS} -static-libgcc -static-libstdc++ -static")

add_executable(
        ${PROJECT_NAME}
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