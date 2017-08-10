cmake_minimum_required(VERSION 3.8)
project(trashy_server)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES src/main.cpp src/config.h src/creat_socket.cpp src/creat_socket.h src/my_str.cpp src/my_str.h)
add_executable(trashy_server ${SOURCE_FILES})