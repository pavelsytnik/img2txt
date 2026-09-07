# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
# Copyright (c) 2026 Pavlo Sytnyk.                                      #
# Licensed under the MIT License. See LICENSE for license information.  #
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #

cmake_minimum_required(VERSION 3.10)
project(img2txt LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED TRUE)
set(CMAKE_C_EXTENSIONS OFF)

if(MSVC)
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
    add_compile_options(/utf-8)
endif()

add_subdirectory(third_party/stb_image)

add_executable(img2txt
    src/main.c
    src/args.c
    src/image.c
    src/image_transform.c
    src/text_art.c
    src/util.c
)

target_link_libraries(img2txt PRIVATE stb_image)
