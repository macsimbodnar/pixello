file(GLOB SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cpp)

add_library(pixello STATIC ${SRCS})

target_include_directories(pixello SYSTEM PRIVATE ../3rd_parties/SDL/include)
target_link_libraries(pixello PRIVATE SDL2main SDL2-static)