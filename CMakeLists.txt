cmake_minimum_required(VERSION 3.5)
project(sex)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "bin")

include(cmake/sanitizers.cmake)

add_subdirectory(sex)
add_subdirectory(third_party)
