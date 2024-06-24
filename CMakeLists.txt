cmake_minimum_required(VERSION 3.21)
project(snake)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -g -std=c++20")
file(GLOB game_src *.h *.cpp)

add_executable(${PROJECT_NAME} ${game_src} )


