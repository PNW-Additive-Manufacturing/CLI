cmake_minimum_required(VERSION 3.5.0)
project(ConsoleApplication VERSION 0.1.0 LANGUAGES C)

add_library(cJSON cJSON/cJSON.c cJSON/cJSON.h)
add_library(cTable cTable/src/table.c cTable/src/table.h)
add_executable(ConsoleApplication program.c)

include(CTest)
enable_testing()