cmake_minimum_required(VERSION 3.10)

project(lesson2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

message("Compiler ID: '${CMAKE_CXX_COMPILER_ID}'")
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
#    add_compile_options("$<$<CONFIG:Release>:-W;-Wall;-O3;-pedantic>")
#    add_compile_options("$<$<CONFIG:Debug>:-W;-Wall;-O0;-pedantic>")
     add_compile_options("-W;-Wall;-O3;-pedantic")
endif()

file(GLOB SOURCES *.h *.cpp)

add_executable(${PROJECT_NAME} ${SOURCES})
