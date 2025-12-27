cmake_minimum_required(VERSION 3.10)
project(polar_code LANGUAGES C)
set(EXEC_NAME hello)

SET(CMAKE_MESSAGE_LOG_LEVEL "STATUS")

SET(CMAKE_C_FLAGS_DEBUG "-Wall -Wshadow -Wformat=2 -Wfloat-equal -Wundef -Wconversion -Wextra -Werror -O0 -g -fno-omit-frame-pointer")
SET(CMAKE_C_FLAGS_RELEASE "-Ofast -ftree-vectorize -DNDEBUG")

# Set C standards
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Debug Configuration: ${CMAKE_C_FLAGS_DEBUG}")
message(STATUS "Release Configuration: ${CMAKE_C_FLAGS_RELEASE}")

# Collect all .c and .cpp files from src directory
file(GLOB SRC_FILES
        "${PROJECT_SOURCE_DIR}/C/SCD/*.c"
)

# Create executable
add_executable(${EXEC_NAME} ${SRC_FILES})

# Include directories
target_include_directories(${EXEC_NAME} PRIVATE
        "${PROJECT_SOURCE_DIR}/C/"
)

# Link MyStaticLib to MyApp
target_link_libraries(${EXEC_NAME} PRIVATE -lm)
