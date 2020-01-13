cmake_minimum_required(VERSION 3.15)
project(server C)

set(CMAKE_C_STANDARD 99)

cmake_minimum_required (VERSION 2.6)
find_package (Threads)

add_executable(server main.c constants.h clients_thread.c clients_thread.h main.h list.c list.h)
target_link_libraries (server ${CMAKE_THREAD_LIBS_INIT})