# Verify cmake version
cmake_minimum_required(VERSION 3.25 FATAL_ERROR)

# Enable testing
include(CTest)
enable_testing()

# Add the subdirectories
add_subdirectory(test_commands)
add_subdirectory(test_monitor)
add_subdirectory(test_utils)

# Add the tests
add_test(NAME "ShellCommands_test" COMMAND test_shell_commands WORKING_DIRECTORY ${CMAKE_BINARY_DIR})

add_test(NAME "MonitorCommands_test" COMMAND test_monitor_commands WORKING_DIRECTORY ${CMAKE_BINARY_DIR})

add_test(NAME "UtilsMethods_test" COMMAND test_utils WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
