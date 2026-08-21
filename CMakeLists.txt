cmake_minimum_required(VERSION 3.23)
project(dustbunny C)

set(CMAKE_C_STANDARD 17)
include_directories(.)

set(PROJECT_FLAGS
    -Wall
    -Wextra
    -Werror
    -fsanitize=address,undefined
    -O
    -g
)

add_executable(dustbunny 
    src/headers/input.h
    src/headers/memory.h
    src/headers/command.h
    src/headers/version.h
    src/command.c
    src/input.c
    src/main.c 
    src/memory.c 
)

