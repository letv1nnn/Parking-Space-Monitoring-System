cmake_minimum_required(VERSION 3.10)
project(FreeParkingSlotDetection)

set(CMAKE_CXX_STANDARD 20)

find_package(OpenCV REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(FreeParkingSlotDetection main.cpp common.h pointsStorage.cpp calibration.cpp freeSpaceDetection.cpp)

target_link_libraries(FreeParkingSlotDetection ${OpenCV_LIBS})

add_custom_command(TARGET FreeParkingSlotDetection POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/test
        ${CMAKE_CURRENT_BINARY_DIR}/test
)
