# Verify cmake version
cmake_minimum_required(VERSION 3.25 FATAL_ERROR)

# Include headers
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/ ../../include)

# Set compiler and linker flags for coverage
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -fprofile-arcs -ftest-coverage --coverage")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -lgcov --coverage")

# Create the executable for tests_commands
add_executable(test_monitor_commands test_monitor_commands.c  ../../src/monitor_commands.c)

# Link the libraries
target_link_libraries(test_monitor_commands PRIVATE cjson::cjson unity::unity gcov)

# Set the output directory
set_target_properties(test_monitor_commands PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
