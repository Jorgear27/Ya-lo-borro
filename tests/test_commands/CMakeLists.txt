# Verify cmake version
cmake_minimum_required(VERSION 3.25 FATAL_ERROR)

# Include headers
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/ ../../include)

# Set compiler and linker flags for coverage
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -fprofile-arcs -ftest-coverage --coverage")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -lgcov --coverage")

# Create the executable for tests_commands
add_executable(test_cd test_cd.c  ../../src/commands.c  ../../src/utils.c  ../../src/monitor_commands.c)
# add_executable(test_echo test_echo.c  ../../src/commands.c  ../../src/utils.c  ../../src/monitor_commands.c)
# add_executable(test_help test_help.c  ../../src/commands.c  ../../src/utils.c  ../../src/monitor_commands.c)
# add_executable(test_internal_command test_internal_command.c  ../../src/commands.c  ../../src/utils.c  ../../src/monitor_commands.c)
# add_executable(test_execute_command test_execute_command.c  ../../src/commands.c  ../../src/utils.c  ../../src/monitor_commands.c)
# add_executable(test_execute_program test_execute_program.c  ../../src/commands.c  ../../src/utils.c  ../../src/monitor_commands.c)
# add_executable(test_piped_commands test_piped_commands.c  ../../src/commands.c  ../../src/utils.c  ../../src/monitor_commands.c)
# add_executable(test_parse_input test_parse_input.c  ../../src/commands.c  ../../src/utils.c  ../../src/monitor_commands.c)
# add_executable(test_parse_pipes test_parse_pipes.c  ../../src/commands.c  ../../src/utils.c  ../../src/monitor_commands.c)
# add_executable(test_quit_shell test_quit_shell.c  ../../src/commands.c  ../../src/utils.c  ../../src/monitor_commands.c)
# add_executable(test_batch_file test_batch_file.c  ../../src/commands.c  ../../src/utils.c  ../../src/monitor_commands.c)

# Link the libraries
target_link_libraries(test_cd PRIVATE cjson::cjson unity::unity gcov)
# target_link_libraries(test_echo PRIVATE cjson::cjson unity::unity gcov)
# target_link_libraries(test_help PRIVATE cjson::cjson unity::unity gcov)
# target_link_libraries(test_internal_command PRIVATE cjson::cjson unity::unity gcov)
# target_link_libraries(test_execute_command PRIVATE cjson::cjson unity::unity gcov)
# target_link_libraries(test_execute_program PRIVATE cjson::cjson unity::unity gcov)
# target_link_libraries(test_piped_commands PRIVATE cjson::cjson unity::unity gcov)
# target_link_libraries(test_parse_input PRIVATE cjson::cjson unity::unity gcov)
# target_link_libraries(test_parse_pipes PRIVATE cjson::cjson unity::unity gcov)
# target_link_libraries(test_quit_shell PRIVATE cjson::cjson unity::unity gcov)
# target_link_libraries(test_batch_file PRIVATE cjson::cjson unity::unity gcov)

# Set the output directory
set_target_properties(test_cd PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
# set_target_properties(test_echo PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
# set_target_properties(test_help PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
# set_target_properties(test_internal_command PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
# set_target_properties(test_execute_command PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
# set_target_properties(test_execute_program PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
# set_target_properties(test_piped_commands PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
# set_target_properties(test_parse_input PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
# set_target_properties(test_parse_pipes PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
# set_target_properties(test_quit_shell PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
# set_target_properties(test_batch_file PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
