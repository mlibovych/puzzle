cmake_minimum_required(VERSION 3.18.2)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED on)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -pipe -Wall -Wextra -Wpedantic")

set(CMAKE_CXX_EXTENSIONS OFF)

set(TARGET puzzle)

project(${TARGET} LANGUAGES CXX OBJCXX)

add_subdirectory(./minikit)

add_subdirectory(./game)


