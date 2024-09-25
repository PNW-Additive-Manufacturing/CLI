cmake_minimum_required(VERSION 3.5.0)
project(ConsoleApplication VERSION 0.1.0 LANGUAGES C)

add_library(cJSON cJSON.c cJSON.h)
add_executable(ConsoleApplication program.c)

include(CTest)
enable_testing()