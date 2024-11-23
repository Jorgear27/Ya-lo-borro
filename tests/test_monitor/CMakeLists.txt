# Verify cmake version
cmake_minimum_required(VERSION 3.25 FATAL_ERROR)

# Include headers
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/ ../../include)

# Set compiler and linker flags for coverage
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -fprofile-arcs -ftest-coverage --coverage")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -lgcov --coverage")

# Create the executable for tests_commands
add_executable(test_start_monitor test_start_monitor.c  ../../src/monitor_commands.c)
add_executable(test_stop_monitor test_stop_monitor.c  ../../src/monitor_commands.c)
add_executable(test_monitor_command test_monitor_command.c  ../../src/monitor_commands.c)
add_executable(test_show_config test_show_config.c  ../../src/monitor_commands.c)
add_executable(test_update_config test_update_config.c  ../../src/monitor_commands.c)
add_executable(test_status_monitor test_status_monitor.c  ../../src/monitor_commands.c)

# Link the libraries
target_link_libraries(test_start_monitor PRIVATE cjson::cjson unity::unity gcov)
target_link_libraries(test_stop_monitor PRIVATE cjson::cjson unity::unity gcov)
target_link_libraries(test_monitor_command PRIVATE cjson::cjson unity::unity gcov)
target_link_libraries(test_show_config PRIVATE cjson::cjson unity::unity gcov)
target_link_libraries(test_update_config PRIVATE cjson::cjson unity::unity gcov)
target_link_libraries(test_status_monitor PRIVATE cjson::cjson unity::unity gcov)

# Set the output directory
set_target_properties(test_start_monitor PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set_target_properties(test_stop_monitor PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set_target_properties(test_monitor_command PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set_target_properties(test_show_config PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set_target_properties(test_update_config PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set_target_properties(test_status_monitor PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
