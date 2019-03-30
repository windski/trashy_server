cmake_minimum_required(VERSION 3.7)
project(trashy_server)

set(CMAKE_CXX_STANDARD 11)


set(SOURCE_FILES src/main.cpp src/config.h src/creat_socket.cpp src/creat_socket.h src/tool_util.cpp src/tool_util.h
        src/Server.cpp src/Server.h src/status_code.h src/log.h src/script_parse.cpp src/script_parse.h src/templates/search_dir.hxx)

add_subdirectory(src/lib)

add_executable(trashy_server ${SOURCE_FILES} src/templates/search_dir.hxx)
link_directories(${PROJECT_BINARY_DIR}/lib)
