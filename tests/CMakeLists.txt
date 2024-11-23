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
add_test(NAME "ChangeDirectory_test" COMMAND test_cd WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
add_test(NAME "Echo_test" COMMAND test_echo WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
add_test(NAME "Help_test" COMMAND test_help WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
add_test(NAME "InternalCommand_test" COMMAND test_internal_command WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
add_test(NAME "ExecuteCommand_test" COMMAND test_execute_command WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
add_test(NAME "ExecuteProgram_test" COMMAND test_execute_program WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
add_test(NAME "PipedCommands_test" COMMAND test_piped_commands WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
add_test(NAME "ParseInput_test" COMMAND test_parse_input WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
add_test(NAME "ParsePipes_test" COMMAND test_parse_pipes WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
add_test(NAME "QuitShell_test" COMMAND test_quit_shell WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
add_test(NAME "BatchFile_test" COMMAND test_batch_file WORKING_DIRECTORY ${CMAKE_BINARY_DIR})

add_test(NAME "StartMonitor_test" COMMAND test_start_monitor WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
add_test(NAME "StopMonitor_test" COMMAND test_stop_monitor WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
add_test(NAME "MonitorCommand_test" COMMAND test_monitor_command WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
add_test(NAME "ShowConfig_test" COMMAND test_show_config WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
add_test(NAME "UpdateConfig_test" COMMAND test_update_config WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
add_test(NAME "StatusMonitor_test" COMMAND test_status_monitor WORKING_DIRECTORY ${CMAKE_BINARY_DIR})

add_test(NAME "Prompt_test" COMMAND test_prompt WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
add_test(NAME "PrintJobs_test" COMMAND test_print_jobs WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
add_test(NAME "WelcomeMessage_test" COMMAND test_welcome_message WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
