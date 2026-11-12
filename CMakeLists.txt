# CMakeLists.txt
# this comment is here so i can easily identify

# the file when i cant look at the filename
cmake_minimum_required(VERSION 3.23)

project(dustbunny LANGUAGES C)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)

option(ENABLE_SANITIZERS "Enable memory sanitizers" ON)

if(ENABLE_SANITIZERS)
    add_compile_options(
        -fsanitize=address,undefined
        -fno-omit-frame-pointer
        -g
    )

    add_link_options(
        -fsanitize=address,undefined
    )
endif()

add_library(strpp STATIC IMPORTED GLOBAL)

set_target_properties(strpp PROPERTIES
    IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/libs/libstrpp.a"
    INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/include" 
)

add_executable(dustbunny
	src/lexer.c
	src/debug.c
	src/nodes.c
	src/main.c
	src/builtins.c
	src/tokens.c
	src/debug.c
	src/variables.c
	src/preproc.c
)

target_include_directories(dustbunny PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(dustbunny PRIVATE 
	readline
)

include(GNUInstallDirs)

install(TARGETS dustbunny
	RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

