add_executable(pixello_test test.cpp)

target_include_directories(pixello_test SYSTEM PRIVATE ../src)

target_link_libraries(pixello_test PRIVATE pixello)
add_test(NAME pixello_test COMMAND ${CMAKE_CURRENT_BINARY_DIR}/pixello_test)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/assets 
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
