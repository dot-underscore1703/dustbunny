cmake_minimum_required(VERSION 3.23)

project(dustbunny LANGUAGES C)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_executable(dustbunny
    src/headers/memory.h
    src/headers/command.h
    src/headers/version.h
    src/headers/builtins.h
    src/command.c
    src/main.c
    src/memory.c
    src/builtins.c
)

target_link_libraries(dustbunny PRIVATE readline)

include(GNUInstallDirs)

install(TARGETS dustbunny
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

