# Verify cmake version
cmake_minimum_required(VERSION 3.25 FATAL_ERROR)

# Include headers
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/ ../../include)

# Set compiler and linker flags for coverage
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -fprofile-arcs -ftest-coverage --coverage")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -lgcov --coverage")

# Create the executable for tests_commands
add_executable(test_prompt test_prompt.c  ../../src/utils.c)
# add_executable(test_print_jobs test_print_jobs.c  ../../src/utils.c)
# add_executable(test_welcome_message test_welcome_message.c  ../../src/utils.c)

# Link the libraries
target_link_libraries(test_prompt PRIVATE cjson::cjson unity::unity gcov)
# target_link_libraries(test_print_jobs PRIVATE cjson::cjson unity::unity gcov)
# target_link_libraries(test_welcome_message PRIVATE cjson::cjson unity::unity gcov)

# Set the output directory
set_target_properties(test_prompt PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
# set_target_properties(test_print_jobs PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
# set_target_properties(test_welcome_message PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
