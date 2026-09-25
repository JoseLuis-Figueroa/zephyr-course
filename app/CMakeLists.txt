cmake_minimum_required(VERSION 3.20.0)

# Add the current source directory(app/boards/our_board) to the list of board root directories
list(APPEND BOARD_ROOT ${CMAKE_CURRENT_SOURCE_DIR})
list(APPEND DTS_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/..)

find_package(Zephyr REQUIRED)
project(my_zephyr_app)

target_sources(app PRIVATE src/main.cpp)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../drivers/our_driver ${CMAKE_CURRENT_BINARY_DIR}/our_driver)