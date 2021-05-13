cmake_minimum_required(VERSION 3.18.2)

SET(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED on)

SET(CMAKE_CXX_FLAGS "-std=c++17 -Wall -Wextra -Wpedantic")

set(TARGET puzzle)

project(${TARGET} LANGUAGES CXX)

set (SOURCES src/main.cpp src/Game.cpp src/state_machine/StateMachine.cpp src/Tetromino.cpp
    src/EventSystem.cpp src/Settings.cpp)

include_directories(inc inc/state_machine)

add_executable(${TARGET} ${SOURCES})

target_link_libraries(${TARGET} PRIVATE MiniKit::MiniKit)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/..")

set_target_properties(${TARGET} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})

add_custom_command(TARGET ${TARGET} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/game/assets $<TARGET_FILE_DIR:${TARGET}>/assets)
