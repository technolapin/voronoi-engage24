cmake_minimum_required(VERSION 3.20)
project(voronoi-weight CXX)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/modules/")
set(ALGEBRAS_LIB_PATH "${CMAKE_SOURCE_DIR}/dependencies")
message(STATUS "Modules are at ${CMAKE_MODULE_PATH}")
message(STATUS "Algebras are at ${ALGEBRAS_LIB_PATH}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -O2 -std=c++20")
add_executable(main src/main.cpp)

add_subdirectory("${ALGEBRAS_LIB_PATH}/garamon_c2ga/" algebras)
target_link_libraries(main PUBLIC c2ga)
