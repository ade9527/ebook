cmake_minimum_required(VERSION 2.8)

project(e-book)

set(CMAKE_C_FLAGS "-std=c99")

aux_source_directory(. DIR_SRCS)

include_directories(include)
add_subdirectory(display)
add_subdirectory(draw)
add_subdirectory(file_encoding)
add_subdirectory(font)
add_executable(e-book ${DIR_SRCS})
target_link_libraries(e-book display draw file_enconding font)
