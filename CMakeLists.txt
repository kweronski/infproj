cmake_minimum_required(VERSION 3.25)
project(infproj LANGUAGES CXX)

include(CTest)

set(exedir "${CMAKE_BINARY_DIR}/${CMAKE_PROJECT_NAME}")

set(CMAKE_EXPORT_COMPILE_COMMANDS 1)
set(CMAKE_CXX_STANDARD_REQUIRED 1)
set(CMAKE_CXX_STANDARD 20)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
add_subdirectory(res)
add_subdirectory(src)
