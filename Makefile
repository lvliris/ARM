# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 2.8

# Default target executed when no arguments are given to make.
default_target: all
.PHONY : default_target

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

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/topeet/Linux+QT/projects/master-host

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/topeet/Linux+QT/projects/master-host

#=============================================================================
# Targets provided globally by CMake.

# Special rule for the target edit_cache
edit_cache:
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --cyan "Running interactive CMake command-line interface..."
	/usr/bin/cmake -i .
.PHONY : edit_cache

# Special rule for the target edit_cache
edit_cache/fast: edit_cache
.PHONY : edit_cache/fast

# Special rule for the target rebuild_cache
rebuild_cache:
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --cyan "Running CMake to regenerate build system..."
	/usr/bin/cmake -H$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR)
.PHONY : rebuild_cache

# Special rule for the target rebuild_cache
rebuild_cache/fast: rebuild_cache
.PHONY : rebuild_cache/fast

# The main all target
all: cmake_check_build_system
	$(CMAKE_COMMAND) -E cmake_progress_start /home/topeet/Linux+QT/projects/master-host/CMakeFiles /home/topeet/Linux+QT/projects/master-host/CMakeFiles/progress.marks
	$(MAKE) -f CMakeFiles/Makefile2 all
	$(CMAKE_COMMAND) -E cmake_progress_start /home/topeet/Linux+QT/projects/master-host/CMakeFiles 0
.PHONY : all

# The main clean target
clean:
	$(MAKE) -f CMakeFiles/Makefile2 clean
.PHONY : clean

# The main clean target
clean/fast: clean
.PHONY : clean/fast

# Prepare targets for installation.
preinstall: all
	$(MAKE) -f CMakeFiles/Makefile2 preinstall
.PHONY : preinstall

# Prepare targets for installation.
preinstall/fast:
	$(MAKE) -f CMakeFiles/Makefile2 preinstall
.PHONY : preinstall/fast

# clear depends
depend:
	$(CMAKE_COMMAND) -H$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR) --check-build-system CMakeFiles/Makefile.cmake 1
.PHONY : depend

#=============================================================================
# Target rules for targets named master

# Build rule for target.
master: cmake_check_build_system
	$(MAKE) -f CMakeFiles/Makefile2 master
.PHONY : master

# fast build rule for target.
master/fast:
	$(MAKE) -f CMakeFiles/master.dir/build.make CMakeFiles/master.dir/build
.PHONY : master/fast

src/converter.o: src/converter.cpp.o
.PHONY : src/converter.o

# target to build an object file
src/converter.cpp.o:
	$(MAKE) -f CMakeFiles/master.dir/build.make CMakeFiles/master.dir/src/converter.cpp.o
.PHONY : src/converter.cpp.o

src/converter.i: src/converter.cpp.i
.PHONY : src/converter.i

# target to preprocess a source file
src/converter.cpp.i:
	$(MAKE) -f CMakeFiles/master.dir/build.make CMakeFiles/master.dir/src/converter.cpp.i
.PHONY : src/converter.cpp.i

src/converter.s: src/converter.cpp.s
.PHONY : src/converter.s

# target to generate assembly for a file
src/converter.cpp.s:
	$(MAKE) -f CMakeFiles/master.dir/build.make CMakeFiles/master.dir/src/converter.cpp.s
.PHONY : src/converter.cpp.s

src/httpclient.o: src/httpclient.cpp.o
.PHONY : src/httpclient.o

# target to build an object file
src/httpclient.cpp.o:
	$(MAKE) -f CMakeFiles/master.dir/build.make CMakeFiles/master.dir/src/httpclient.cpp.o
.PHONY : src/httpclient.cpp.o

src/httpclient.i: src/httpclient.cpp.i
.PHONY : src/httpclient.i

# target to preprocess a source file
src/httpclient.cpp.i:
	$(MAKE) -f CMakeFiles/master.dir/build.make CMakeFiles/master.dir/src/httpclient.cpp.i
.PHONY : src/httpclient.cpp.i

src/httpclient.s: src/httpclient.cpp.s
.PHONY : src/httpclient.s

# target to generate assembly for a file
src/httpclient.cpp.s:
	$(MAKE) -f CMakeFiles/master.dir/build.make CMakeFiles/master.dir/src/httpclient.cpp.s
.PHONY : src/httpclient.cpp.s

src/main.o: src/main.cpp.o
.PHONY : src/main.o

# target to build an object file
src/main.cpp.o:
	$(MAKE) -f CMakeFiles/master.dir/build.make CMakeFiles/master.dir/src/main.cpp.o
.PHONY : src/main.cpp.o

src/main.i: src/main.cpp.i
.PHONY : src/main.i

# target to preprocess a source file
src/main.cpp.i:
	$(MAKE) -f CMakeFiles/master.dir/build.make CMakeFiles/master.dir/src/main.cpp.i
.PHONY : src/main.cpp.i

src/main.s: src/main.cpp.s
.PHONY : src/main.s

# target to generate assembly for a file
src/main.cpp.s:
	$(MAKE) -f CMakeFiles/master.dir/build.make CMakeFiles/master.dir/src/main.cpp.s
.PHONY : src/main.cpp.s

src/orderparser.o: src/orderparser.cpp.o
.PHONY : src/orderparser.o

# target to build an object file
src/orderparser.cpp.o:
	$(MAKE) -f CMakeFiles/master.dir/build.make CMakeFiles/master.dir/src/orderparser.cpp.o
.PHONY : src/orderparser.cpp.o

src/orderparser.i: src/orderparser.cpp.i
.PHONY : src/orderparser.i

# target to preprocess a source file
src/orderparser.cpp.i:
	$(MAKE) -f CMakeFiles/master.dir/build.make CMakeFiles/master.dir/src/orderparser.cpp.i
.PHONY : src/orderparser.cpp.i

src/orderparser.s: src/orderparser.cpp.s
.PHONY : src/orderparser.s

# target to generate assembly for a file
src/orderparser.cpp.s:
	$(MAKE) -f CMakeFiles/master.dir/build.make CMakeFiles/master.dir/src/orderparser.cpp.s
.PHONY : src/orderparser.cpp.s

src/orderpoll.o: src/orderpoll.cpp.o
.PHONY : src/orderpoll.o

# target to build an object file
src/orderpoll.cpp.o:
	$(MAKE) -f CMakeFiles/master.dir/build.make CMakeFiles/master.dir/src/orderpoll.cpp.o
.PHONY : src/orderpoll.cpp.o

src/orderpoll.i: src/orderpoll.cpp.i
.PHONY : src/orderpoll.i

# target to preprocess a source file
src/orderpoll.cpp.i:
	$(MAKE) -f CMakeFiles/master.dir/build.make CMakeFiles/master.dir/src/orderpoll.cpp.i
.PHONY : src/orderpoll.cpp.i

src/orderpoll.s: src/orderpoll.cpp.s
.PHONY : src/orderpoll.s

# target to generate assembly for a file
src/orderpoll.cpp.s:
	$(MAKE) -f CMakeFiles/master.dir/build.make CMakeFiles/master.dir/src/orderpoll.cpp.s
.PHONY : src/orderpoll.cpp.s

src/uart.o: src/uart.cpp.o
.PHONY : src/uart.o

# target to build an object file
src/uart.cpp.o:
	$(MAKE) -f CMakeFiles/master.dir/build.make CMakeFiles/master.dir/src/uart.cpp.o
.PHONY : src/uart.cpp.o

src/uart.i: src/uart.cpp.i
.PHONY : src/uart.i

# target to preprocess a source file
src/uart.cpp.i:
	$(MAKE) -f CMakeFiles/master.dir/build.make CMakeFiles/master.dir/src/uart.cpp.i
.PHONY : src/uart.cpp.i

src/uart.s: src/uart.cpp.s
.PHONY : src/uart.s

# target to generate assembly for a file
src/uart.cpp.s:
	$(MAKE) -f CMakeFiles/master.dir/build.make CMakeFiles/master.dir/src/uart.cpp.s
.PHONY : src/uart.cpp.s

# Help Target
help:
	@echo "The following are some of the valid targets for this Makefile:"
	@echo "... all (the default if no target is provided)"
	@echo "... clean"
	@echo "... depend"
	@echo "... edit_cache"
	@echo "... master"
	@echo "... rebuild_cache"
	@echo "... src/converter.o"
	@echo "... src/converter.i"
	@echo "... src/converter.s"
	@echo "... src/httpclient.o"
	@echo "... src/httpclient.i"
	@echo "... src/httpclient.s"
	@echo "... src/main.o"
	@echo "... src/main.i"
	@echo "... src/main.s"
	@echo "... src/orderparser.o"
	@echo "... src/orderparser.i"
	@echo "... src/orderparser.s"
	@echo "... src/orderpoll.o"
	@echo "... src/orderpoll.i"
	@echo "... src/orderpoll.s"
	@echo "... src/uart.o"
	@echo "... src/uart.i"
	@echo "... src/uart.s"
.PHONY : help



#=============================================================================
# Special targets to cleanup operation of make.

# Special rule to run CMake to check the build system integrity.
# No rule that depends on this can have commands that come from listfiles
# because they might be regenerated.
cmake_check_build_system:
	$(CMAKE_COMMAND) -H$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR) --check-build-system CMakeFiles/Makefile.cmake 0
.PHONY : cmake_check_build_system

