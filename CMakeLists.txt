cmake_minimum_required(VERSION 3.18)

project(pixello)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(SDL_STATIC ON CACHE BOOL "" FORCE)
set(SDL_SHARED OFF CACHE BOOL "" FORCE)

set(CMAKE_CXX_FLAGS "-Wall -Wextra -O3")
# set(CMAKE_CXX_FLAGS_DEBUG "-g")
# set(CMAKE_CXX_FLAGS_RELEASE "-O3")


add_subdirectory(src)
add_subdirectory(3rd_parties)

enable_testing()
add_subdirectory(tests)
