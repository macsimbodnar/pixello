include_directories(${CMAKE_SOURCE_DIR}/src)
add_executable(pixello_test test.cpp)

target_include_directories(pixello_test SYSTEM PRIVATE ${CMAKE_SOURCE_DIR}/3rd_parties/SDL/include)

target_link_libraries (pixello_test PRIVATE pixello)
add_test (NAME pixello_test COMMAND ${CMAKE_CURRENT_BINARY_DIR}/pixello_test)