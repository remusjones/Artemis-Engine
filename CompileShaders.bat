@echo off
set VULKAN_SDK=%VULKAN_SDK%
set OUTPUT_FOLDER=cmake-build-debug/Shaders
if not exist "%OUTPUT_FOLDER%" md "%OUTPUT_FOLDER%"
@echo on
for /r %%i in (*.frag, *.vert) do %VULKAN_SDK%\Bin\glslc %%i -o %OUTPUT_FOLDER%/%%~ni.spv