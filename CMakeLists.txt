# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
# Copyright (c) 2026 Pavlo Sytnyk.                                      #
# Licensed under the MIT License. See LICENSE for license information.  #
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #

cmake_minimum_required(VERSION 3.10)
project(img2txt LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED TRUE)
set(CMAKE_C_EXTENSIONS OFF)

add_subdirectory(third-party/stb_image)

add_executable(img2txt src/main.c)

target_link_libraries(img2txt PRIVATE stb_image)
