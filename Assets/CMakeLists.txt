message("Copying Models")
add_subdirectory(Models)
message("Copying Textures")
add_subdirectory(Textures)
message("Compiling Shaders")
# Compile SPIR-V Shaders
add_subdirectory(Shaders)
