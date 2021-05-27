cmake_minimum_required(VERSION 3.14)
project(my_project)

# GoogleTest requires at least C++11
set(CMAKE_CXX_STANDARD 17)

include(FetchContent)
FetchContent_Declare(
	googletest
	URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(
	lups_test
	tests.cpp
	token.h
	lexer.cpp
	lexer.h
	parser.cpp
	parser.h
	ast.h
	ast.cpp
	object.h
	eval.cpp
	eval.h
	code.h
	code.cpp
)
target_link_libraries(
	lups_test
	gtest_main
)

include(GoogleTest)
gtest_discover_tests(lups_test)
