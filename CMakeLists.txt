cmake_minimum_required(VERSION 3.10)
project(FreeParkingSlotsDetector)

set(CMAKE_CXX_STANDARD 20)

find_package(OpenCV REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(FreeParkingSlotsDetector main.cpp
        common.h
        debug.cpp)

target_link_libraries(FreeParkingSlotsDetector ${OpenCV_LIBS})

add_custom_command(TARGET FreeParkingSlotsDetector POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/test
        ${CMAKE_CURRENT_BINARY_DIR}/test
)