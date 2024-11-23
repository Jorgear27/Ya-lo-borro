# Verify cmake version
cmake_minimum_required(VERSION 3.25 FATAL_ERROR)

# Create project
project (metrics VERSION 1.0.0 DESCRIPTION "A monitoring system" LANGUAGES C)

# Flags for compiling
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_FLAGS_DEBUG "-g -O0 -Wall -Wpedantic -Wextra -Wunused-parameter")

# Include headers
include_directories(include)

# Find dependencies en Conan
find_package(cJSON REQUIRED)

# Añadir los archivos fuente
set(SOURCES
    src/main.c
    src/metrics.c
    src/expose_metrics.c
)

# Define the executable target
add_executable(${PROJECT_NAME} ${SOURCES})

# Set include directories (equivalent to CFLAGS)
target_include_directories(${PROJECT_NAME} PRIVATE include /usr/local/include)

# Link libraries (equivalent to LDFLAGS)
target_link_libraries(${PROJECT_NAME} PRIVATE cjson::cjson promhttp prom)
