# CMAKE generated file: DO NOT EDIT!
# Generated by "MinGW Makefiles" Generator, CMake Version 3.20

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

SHELL = cmd.exe

# The CMake executable.
CMAKE_COMMAND = "D:\Work\CLion 2021.2.3\bin\cmake\win\bin\cmake.exe"

# The command to remove a file.
RM = "D:\Work\CLion 2021.2.3\bin\cmake\win\bin\cmake.exe" -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = D:\Work\Git\3DEngine

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = D:\Work\Git\3DEngine\cmake-build-debug

# Utility rule file for uninstall.

# Include any custom commands dependencies for this target.
include glfw_local/CMakeFiles/uninstall.dir/compiler_depend.make

# Include the progress variables for this target.
include glfw_local/CMakeFiles/uninstall.dir/progress.make

glfw_local/CMakeFiles/uninstall:
	cd /d D:\Work\Git\3DEngine\cmake-build-debug\glfw_local && "D:\Work\CLion 2021.2.3\bin\cmake\win\bin\cmake.exe" -P D:/Work/Git/3DEngine/cmake-build-debug/glfw_local/cmake_uninstall.cmake

uninstall: glfw_local/CMakeFiles/uninstall
uninstall: glfw_local/CMakeFiles/uninstall.dir/build.make
.PHONY : uninstall

# Rule to build all files generated by this target.
glfw_local/CMakeFiles/uninstall.dir/build: uninstall
.PHONY : glfw_local/CMakeFiles/uninstall.dir/build

glfw_local/CMakeFiles/uninstall.dir/clean:
	cd /d D:\Work\Git\3DEngine\cmake-build-debug\glfw_local && $(CMAKE_COMMAND) -P CMakeFiles\uninstall.dir\cmake_clean.cmake
.PHONY : glfw_local/CMakeFiles/uninstall.dir/clean

glfw_local/CMakeFiles/uninstall.dir/depend:
	$(CMAKE_COMMAND) -E cmake_depends "MinGW Makefiles" D:\Work\Git\3DEngine D:\Work\Git\3DEngine\glfw D:\Work\Git\3DEngine\cmake-build-debug D:\Work\Git\3DEngine\cmake-build-debug\glfw_local D:\Work\Git\3DEngine\cmake-build-debug\glfw_local\CMakeFiles\uninstall.dir\DependInfo.cmake --color=$(COLOR)
.PHONY : glfw_local/CMakeFiles/uninstall.dir/depend

