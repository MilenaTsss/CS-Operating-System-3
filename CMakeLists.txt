cmake_minimum_required(VERSION 3.20)
project(osi3 C)

set(CMAKE_C_STANDARD 11)

add_executable(osi3 main.c server.c client.c helper.h handleClient.c helper.c war.h)
