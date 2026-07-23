cmake_minimum_required(VERSION 3.16)
project(main C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)  # needed for GNU computed-goto (labels-as-values)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

set(CMAKE_C_FLAGS_DEBUG "-g -O0 -Wall -Wextra")
set(CMAKE_C_FLAGS_RELEASE "-O2 -Wall")

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/src/*.c")

add_executable(main ${SOURCES})

target_include_directories(main PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)

find_package(Curses REQUIRED)
target_include_directories(main PRIVATE ${CURSES_INCLUDE_DIR})
target_link_libraries(main PRIVATE ${CURSES_LIBRARIES})

# Symlink compile_commands.json to project root for clangd
add_custom_command(TARGET main POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E create_symlink
        ${CMAKE_BINARY_DIR}/compile_commands.json
        ${CMAKE_SOURCE_DIR}/compile_commands.json
    COMMENT "Linking compile_commands.json for clangd"
)