# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.20

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
CMAKE_SOURCE_DIR = /home/eemil/dev/c++/lups

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/eemil/dev/c++/lups/build

# Include any dependencies generated for this target.
include CMakeFiles/lups_test.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/lups_test.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/lups_test.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/lups_test.dir/flags.make

CMakeFiles/lups_test.dir/tests.cpp.o: CMakeFiles/lups_test.dir/flags.make
CMakeFiles/lups_test.dir/tests.cpp.o: ../tests.cpp
CMakeFiles/lups_test.dir/tests.cpp.o: CMakeFiles/lups_test.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/eemil/dev/c++/lups/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/lups_test.dir/tests.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/lups_test.dir/tests.cpp.o -MF CMakeFiles/lups_test.dir/tests.cpp.o.d -o CMakeFiles/lups_test.dir/tests.cpp.o -c /home/eemil/dev/c++/lups/tests.cpp

CMakeFiles/lups_test.dir/tests.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/lups_test.dir/tests.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/eemil/dev/c++/lups/tests.cpp > CMakeFiles/lups_test.dir/tests.cpp.i

CMakeFiles/lups_test.dir/tests.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/lups_test.dir/tests.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/eemil/dev/c++/lups/tests.cpp -o CMakeFiles/lups_test.dir/tests.cpp.s

CMakeFiles/lups_test.dir/lexer.cpp.o: CMakeFiles/lups_test.dir/flags.make
CMakeFiles/lups_test.dir/lexer.cpp.o: ../lexer.cpp
CMakeFiles/lups_test.dir/lexer.cpp.o: CMakeFiles/lups_test.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/eemil/dev/c++/lups/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/lups_test.dir/lexer.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/lups_test.dir/lexer.cpp.o -MF CMakeFiles/lups_test.dir/lexer.cpp.o.d -o CMakeFiles/lups_test.dir/lexer.cpp.o -c /home/eemil/dev/c++/lups/lexer.cpp

CMakeFiles/lups_test.dir/lexer.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/lups_test.dir/lexer.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/eemil/dev/c++/lups/lexer.cpp > CMakeFiles/lups_test.dir/lexer.cpp.i

CMakeFiles/lups_test.dir/lexer.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/lups_test.dir/lexer.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/eemil/dev/c++/lups/lexer.cpp -o CMakeFiles/lups_test.dir/lexer.cpp.s

CMakeFiles/lups_test.dir/parser.cpp.o: CMakeFiles/lups_test.dir/flags.make
CMakeFiles/lups_test.dir/parser.cpp.o: ../parser.cpp
CMakeFiles/lups_test.dir/parser.cpp.o: CMakeFiles/lups_test.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/eemil/dev/c++/lups/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object CMakeFiles/lups_test.dir/parser.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/lups_test.dir/parser.cpp.o -MF CMakeFiles/lups_test.dir/parser.cpp.o.d -o CMakeFiles/lups_test.dir/parser.cpp.o -c /home/eemil/dev/c++/lups/parser.cpp

CMakeFiles/lups_test.dir/parser.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/lups_test.dir/parser.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/eemil/dev/c++/lups/parser.cpp > CMakeFiles/lups_test.dir/parser.cpp.i

CMakeFiles/lups_test.dir/parser.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/lups_test.dir/parser.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/eemil/dev/c++/lups/parser.cpp -o CMakeFiles/lups_test.dir/parser.cpp.s

CMakeFiles/lups_test.dir/ast.cpp.o: CMakeFiles/lups_test.dir/flags.make
CMakeFiles/lups_test.dir/ast.cpp.o: ../ast.cpp
CMakeFiles/lups_test.dir/ast.cpp.o: CMakeFiles/lups_test.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/eemil/dev/c++/lups/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building CXX object CMakeFiles/lups_test.dir/ast.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/lups_test.dir/ast.cpp.o -MF CMakeFiles/lups_test.dir/ast.cpp.o.d -o CMakeFiles/lups_test.dir/ast.cpp.o -c /home/eemil/dev/c++/lups/ast.cpp

CMakeFiles/lups_test.dir/ast.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/lups_test.dir/ast.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/eemil/dev/c++/lups/ast.cpp > CMakeFiles/lups_test.dir/ast.cpp.i

CMakeFiles/lups_test.dir/ast.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/lups_test.dir/ast.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/eemil/dev/c++/lups/ast.cpp -o CMakeFiles/lups_test.dir/ast.cpp.s

CMakeFiles/lups_test.dir/eval.cpp.o: CMakeFiles/lups_test.dir/flags.make
CMakeFiles/lups_test.dir/eval.cpp.o: ../eval.cpp
CMakeFiles/lups_test.dir/eval.cpp.o: CMakeFiles/lups_test.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/eemil/dev/c++/lups/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Building CXX object CMakeFiles/lups_test.dir/eval.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/lups_test.dir/eval.cpp.o -MF CMakeFiles/lups_test.dir/eval.cpp.o.d -o CMakeFiles/lups_test.dir/eval.cpp.o -c /home/eemil/dev/c++/lups/eval.cpp

CMakeFiles/lups_test.dir/eval.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/lups_test.dir/eval.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/eemil/dev/c++/lups/eval.cpp > CMakeFiles/lups_test.dir/eval.cpp.i

CMakeFiles/lups_test.dir/eval.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/lups_test.dir/eval.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/eemil/dev/c++/lups/eval.cpp -o CMakeFiles/lups_test.dir/eval.cpp.s

CMakeFiles/lups_test.dir/code.cpp.o: CMakeFiles/lups_test.dir/flags.make
CMakeFiles/lups_test.dir/code.cpp.o: ../code.cpp
CMakeFiles/lups_test.dir/code.cpp.o: CMakeFiles/lups_test.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/eemil/dev/c++/lups/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_6) "Building CXX object CMakeFiles/lups_test.dir/code.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/lups_test.dir/code.cpp.o -MF CMakeFiles/lups_test.dir/code.cpp.o.d -o CMakeFiles/lups_test.dir/code.cpp.o -c /home/eemil/dev/c++/lups/code.cpp

CMakeFiles/lups_test.dir/code.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/lups_test.dir/code.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/eemil/dev/c++/lups/code.cpp > CMakeFiles/lups_test.dir/code.cpp.i

CMakeFiles/lups_test.dir/code.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/lups_test.dir/code.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/eemil/dev/c++/lups/code.cpp -o CMakeFiles/lups_test.dir/code.cpp.s

# Object files for target lups_test
lups_test_OBJECTS = \
"CMakeFiles/lups_test.dir/tests.cpp.o" \
"CMakeFiles/lups_test.dir/lexer.cpp.o" \
"CMakeFiles/lups_test.dir/parser.cpp.o" \
"CMakeFiles/lups_test.dir/ast.cpp.o" \
"CMakeFiles/lups_test.dir/eval.cpp.o" \
"CMakeFiles/lups_test.dir/code.cpp.o"

# External object files for target lups_test
lups_test_EXTERNAL_OBJECTS =

lups_test: CMakeFiles/lups_test.dir/tests.cpp.o
lups_test: CMakeFiles/lups_test.dir/lexer.cpp.o
lups_test: CMakeFiles/lups_test.dir/parser.cpp.o
lups_test: CMakeFiles/lups_test.dir/ast.cpp.o
lups_test: CMakeFiles/lups_test.dir/eval.cpp.o
lups_test: CMakeFiles/lups_test.dir/code.cpp.o
lups_test: CMakeFiles/lups_test.dir/build.make
lups_test: lib/libgtest_main.a
lups_test: lib/libgtest.a
lups_test: CMakeFiles/lups_test.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/eemil/dev/c++/lups/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_7) "Linking CXX executable lups_test"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/lups_test.dir/link.txt --verbose=$(VERBOSE)
	/usr/bin/cmake -D TEST_TARGET=lups_test -D TEST_EXECUTABLE=/home/eemil/dev/c++/lups/build/lups_test -D TEST_EXECUTOR= -D TEST_WORKING_DIR=/home/eemil/dev/c++/lups/build -D TEST_EXTRA_ARGS= -D TEST_PROPERTIES= -D TEST_PREFIX= -D TEST_SUFFIX= -D NO_PRETTY_TYPES=FALSE -D NO_PRETTY_VALUES=FALSE -D TEST_LIST=lups_test_TESTS -D CTEST_FILE=/home/eemil/dev/c++/lups/build/lups_test[1]_tests.cmake -D TEST_DISCOVERY_TIMEOUT=5 -D TEST_XML_OUTPUT_DIR= -P /usr/share/cmake-3.20/Modules/GoogleTestAddTests.cmake

# Rule to build all files generated by this target.
CMakeFiles/lups_test.dir/build: lups_test
.PHONY : CMakeFiles/lups_test.dir/build

CMakeFiles/lups_test.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/lups_test.dir/cmake_clean.cmake
.PHONY : CMakeFiles/lups_test.dir/clean

CMakeFiles/lups_test.dir/depend:
	cd /home/eemil/dev/c++/lups/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/eemil/dev/c++/lups /home/eemil/dev/c++/lups /home/eemil/dev/c++/lups/build /home/eemil/dev/c++/lups/build /home/eemil/dev/c++/lups/build/CMakeFiles/lups_test.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/lups_test.dir/depend

