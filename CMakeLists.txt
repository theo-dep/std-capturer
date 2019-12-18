# project's name
project(std-capturer)
set(PROJECT_VERSION 0.1)

cmake_minimum_required (VERSION 3.9)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)
SET(CMAKE_BUILD_TYPE Release)


# Add source to this project's library.
set(library_name StdCapture)
add_library(${library_name} ${library_name}.cpp)

install(TARGETS ${library_name} DESTINATION lib)
install(FILES ${library_name}.h DESTINATION include)

# Test
add_subdirectory(test)
