# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.0

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
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/kane/gazebo

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/kane/gazebo/build

# Include any dependencies generated for this target.
include CMakeFiles/ServerMain.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/ServerMain.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/ServerMain.dir/flags.make

CMakeFiles/ServerMain.dir/src/ServerMain.cc.o: CMakeFiles/ServerMain.dir/flags.make
CMakeFiles/ServerMain.dir/src/ServerMain.cc.o: ../src/ServerMain.cc
	$(CMAKE_COMMAND) -E cmake_progress_report /home/kane/gazebo/build/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object CMakeFiles/ServerMain.dir/src/ServerMain.cc.o"
	/usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/ServerMain.dir/src/ServerMain.cc.o -c /home/kane/gazebo/src/ServerMain.cc

CMakeFiles/ServerMain.dir/src/ServerMain.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/ServerMain.dir/src/ServerMain.cc.i"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/kane/gazebo/src/ServerMain.cc > CMakeFiles/ServerMain.dir/src/ServerMain.cc.i

CMakeFiles/ServerMain.dir/src/ServerMain.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/ServerMain.dir/src/ServerMain.cc.s"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/kane/gazebo/src/ServerMain.cc -o CMakeFiles/ServerMain.dir/src/ServerMain.cc.s

CMakeFiles/ServerMain.dir/src/ServerMain.cc.o.requires:
.PHONY : CMakeFiles/ServerMain.dir/src/ServerMain.cc.o.requires

CMakeFiles/ServerMain.dir/src/ServerMain.cc.o.provides: CMakeFiles/ServerMain.dir/src/ServerMain.cc.o.requires
	$(MAKE) -f CMakeFiles/ServerMain.dir/build.make CMakeFiles/ServerMain.dir/src/ServerMain.cc.o.provides.build
.PHONY : CMakeFiles/ServerMain.dir/src/ServerMain.cc.o.provides

CMakeFiles/ServerMain.dir/src/ServerMain.cc.o.provides.build: CMakeFiles/ServerMain.dir/src/ServerMain.cc.o

# Object files for target ServerMain
ServerMain_OBJECTS = \
"CMakeFiles/ServerMain.dir/src/ServerMain.cc.o"

# External object files for target ServerMain
ServerMain_EXTERNAL_OBJECTS =

ServerMain: CMakeFiles/ServerMain.dir/src/ServerMain.cc.o
ServerMain: CMakeFiles/ServerMain.dir/build.make
ServerMain: /usr/lib/x86_64-linux-gnu/libboost_system.so
ServerMain: /usr/lib/x86_64-linux-gnu/libprotobuf.so
ServerMain: /usr/lib/x86_64-linux-gnu/libQtGui.so
ServerMain: /usr/lib/x86_64-linux-gnu/libQtCore.so
ServerMain: /usr/lib/x86_64-linux-gnu/libboost_system.so
ServerMain: /usr/lib/x86_64-linux-gnu/libprotobuf.so
ServerMain: /usr/lib/x86_64-linux-gnu/libQtGui.so
ServerMain: /usr/lib/x86_64-linux-gnu/libQtCore.so
ServerMain: CMakeFiles/ServerMain.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking CXX executable ServerMain"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/ServerMain.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/ServerMain.dir/build: ServerMain
.PHONY : CMakeFiles/ServerMain.dir/build

CMakeFiles/ServerMain.dir/requires: CMakeFiles/ServerMain.dir/src/ServerMain.cc.o.requires
.PHONY : CMakeFiles/ServerMain.dir/requires

CMakeFiles/ServerMain.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/ServerMain.dir/cmake_clean.cmake
.PHONY : CMakeFiles/ServerMain.dir/clean

CMakeFiles/ServerMain.dir/depend:
	cd /home/kane/gazebo/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/kane/gazebo /home/kane/gazebo /home/kane/gazebo/build /home/kane/gazebo/build /home/kane/gazebo/build/CMakeFiles/ServerMain.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/ServerMain.dir/depend

