# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.22

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

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/patrick/Projects/riscv-sim

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/patrick/Projects/riscv-sim/build

# Include any dependencies generated for this target.
include CMakeFiles/memgen.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/memgen.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/memgen.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/memgen.dir/flags.make

CMakeFiles/memgen.dir/generate.cpp.o: CMakeFiles/memgen.dir/flags.make
CMakeFiles/memgen.dir/generate.cpp.o: ../generate.cpp
CMakeFiles/memgen.dir/generate.cpp.o: CMakeFiles/memgen.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/patrick/Projects/riscv-sim/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/memgen.dir/generate.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/memgen.dir/generate.cpp.o -MF CMakeFiles/memgen.dir/generate.cpp.o.d -o CMakeFiles/memgen.dir/generate.cpp.o -c /home/patrick/Projects/riscv-sim/generate.cpp

CMakeFiles/memgen.dir/generate.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/memgen.dir/generate.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/patrick/Projects/riscv-sim/generate.cpp > CMakeFiles/memgen.dir/generate.cpp.i

CMakeFiles/memgen.dir/generate.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/memgen.dir/generate.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/patrick/Projects/riscv-sim/generate.cpp -o CMakeFiles/memgen.dir/generate.cpp.s

# Object files for target memgen
memgen_OBJECTS = \
"CMakeFiles/memgen.dir/generate.cpp.o"

# External object files for target memgen
memgen_EXTERNAL_OBJECTS =

memgen: CMakeFiles/memgen.dir/generate.cpp.o
memgen: CMakeFiles/memgen.dir/build.make
memgen: CMakeFiles/memgen.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/patrick/Projects/riscv-sim/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable memgen"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/memgen.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/memgen.dir/build: memgen
.PHONY : CMakeFiles/memgen.dir/build

CMakeFiles/memgen.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/memgen.dir/cmake_clean.cmake
.PHONY : CMakeFiles/memgen.dir/clean

CMakeFiles/memgen.dir/depend:
	cd /home/patrick/Projects/riscv-sim/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/patrick/Projects/riscv-sim /home/patrick/Projects/riscv-sim /home/patrick/Projects/riscv-sim/build /home/patrick/Projects/riscv-sim/build /home/patrick/Projects/riscv-sim/build/CMakeFiles/memgen.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/memgen.dir/depend
