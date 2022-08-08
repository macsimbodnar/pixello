add_executable(pixello_test test.cpp)

target_include_directories(pixello_test SYSTEM PRIVATE ../src)

target_link_libraries(pixello_test PRIVATE pixello)
add_test(NAME pixello_test COMMAND ${CMAKE_CURRENT_BINARY_DIR}/pixello_test)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/assets/sample_640x426.bmp 
               ${CMAKE_CURRENT_BINARY_DIR}/assets/sample_640x426.bmp 
               COPYONLY)