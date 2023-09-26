cmake_minimum_required(VERSION 3.0)
project(TCP-Server)

set(SOURCE_FILES
    main.cpp
    server.cpp
)

include_directories(liburing/src/include)

link_directories(liburing/src/)

add_executable(tcp-server ${SOURCE_FILES})

target_link_libraries(tcp-server uring)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -O3 -D_GNU_SOURCE")

