cmake_minimum_required(VERSION 3.29.3)

project(EMULATOR)

set(CMAKE_CXX_STANDARD 20)

add_executable(emulator main.cpp Processor.cpp)
target_include_directories(emulator PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
