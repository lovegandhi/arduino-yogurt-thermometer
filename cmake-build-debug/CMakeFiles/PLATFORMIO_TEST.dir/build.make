# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.7

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = "/Users/Landi/Library/Application Support/JetBrains/Toolbox/apps/CLion/ch-0/171.4694.31/CLion.app/Contents/bin/cmake/bin/cmake"

# The command to remove a file.
RM = "/Users/Landi/Library/Application Support/JetBrains/Toolbox/apps/CLion/ch-0/171.4694.31/CLion.app/Contents/bin/cmake/bin/cmake" -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/Landi/Source/Arduino/thermometer

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/Landi/Source/Arduino/thermometer/cmake-build-debug

# Utility rule file for PLATFORMIO_TEST.

# Include the progress variables for this target.
include CMakeFiles/PLATFORMIO_TEST.dir/progress.make

CMakeFiles/PLATFORMIO_TEST:
	cd /Users/Landi/Source/Arduino/thermometer && /usr/local/bin/platformio -f -c clion test

PLATFORMIO_TEST: CMakeFiles/PLATFORMIO_TEST
PLATFORMIO_TEST: CMakeFiles/PLATFORMIO_TEST.dir/build.make

.PHONY : PLATFORMIO_TEST

# Rule to build all files generated by this target.
CMakeFiles/PLATFORMIO_TEST.dir/build: PLATFORMIO_TEST

.PHONY : CMakeFiles/PLATFORMIO_TEST.dir/build

CMakeFiles/PLATFORMIO_TEST.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/PLATFORMIO_TEST.dir/cmake_clean.cmake
.PHONY : CMakeFiles/PLATFORMIO_TEST.dir/clean

CMakeFiles/PLATFORMIO_TEST.dir/depend:
	cd /Users/Landi/Source/Arduino/thermometer/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/Landi/Source/Arduino/thermometer /Users/Landi/Source/Arduino/thermometer /Users/Landi/Source/Arduino/thermometer/cmake-build-debug /Users/Landi/Source/Arduino/thermometer/cmake-build-debug /Users/Landi/Source/Arduino/thermometer/cmake-build-debug/CMakeFiles/PLATFORMIO_TEST.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/PLATFORMIO_TEST.dir/depend

