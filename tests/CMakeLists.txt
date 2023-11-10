# Test
add_executable(pixello_test test.cpp)

target_include_directories(pixello_test SYSTEM PRIVATE ../src)

target_link_libraries(pixello_test PRIVATE pixello)
add_test(NAME pixello_test COMMAND ${CMAKE_CURRENT_BINARY_DIR}/pixello_test)

# Isomtric
add_executable(isometric isometric.cpp)

target_include_directories(isometric SYSTEM PRIVATE ../src)

target_link_libraries(isometric PRIVATE pixello)
add_test(NAME isometric COMMAND ${CMAKE_CURRENT_BINARY_DIR}/isometric)


# Assets files
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/assets 
DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
