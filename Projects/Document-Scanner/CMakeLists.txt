cmake_minimum_required(VERSION 3.10)

project("Document-Scanner")

set(CMAKE_CXX_STANDARD 20)

find_package(OpenCV REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(DocumentScanner main.cpp)

target_link_libraries(DocumentScanner ${OpenCV_LIBS})

add_custom_command(TARGET DocumentScanner POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/test
        ${CMAKE_CURRENT_BINARY_DIR}/test
)
