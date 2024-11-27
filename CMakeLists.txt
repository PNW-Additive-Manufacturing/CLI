cmake_minimum_required(VERSION 3.5.0)
project(ConsoleApplication VERSION 0.1.0 LANGUAGES C)

add_library(cJSON libs/cJSON/cJSON.c libs/cJSON/cJSON.h)
add_library(cTable libs/ctable/src/table.c libs/ctable/src/table.h)

add_executable(ConsoleApplication program.c)

target_link_libraries(ConsoleApplication PRIVATE cJSON cTable)

include(CTest)
enable_testing()