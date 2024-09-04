cmake_minimum_required(VERSION 3.26)
project(template LANGUAGES CXX)


set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_BUILD_TYPE Debug)
message(CHECK_START ${CMAKE_BUILD_TYPE})

# message(${CMAKE_SOURCE_DIR})

# generate compile_command.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories(${CMAKE_SOURCE_DIR}/include)


enable_testing()

add_subdirectory(src)
add_subdirectory(test)
add_subdirectory(benchmark)
