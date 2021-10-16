cmake_minimum_required(VERSION 3.18)

project(pixello)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(SDL_STATIC ON CACHE BOOL "" FORCE)
set(SDL_SHARED OFF CACHE BOOL "" FORCE)

add_subdirectory(${CMAKE_SOURCE_DIR}/src)
add_subdirectory(${CMAKE_SOURCE_DIR}/3rd_parties)

enable_testing()
add_subdirectory(${CMAKE_SOURCE_DIR}/tests)
