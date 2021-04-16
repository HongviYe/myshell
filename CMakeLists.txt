cmake_minimum_required (VERSION 2.6)
project(myshell)
#best opt
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0 -g")
#
message("building myshell")
#
add_definitions(-D _LIB)
include_directories(./include ./src )
add_executable(myshell
include/myshell.h
include/CommandFactory.h
include/AbstractCommand.h
include/singleton.h
src/CommandFactory.cpp
src/myshell.cpp
src/main.cpp
src/AbstractCommand.cpp
)
