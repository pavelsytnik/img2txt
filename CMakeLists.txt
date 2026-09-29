cmake_minimum_required(VERSION 3.10)
project(img2txt LANGUAGES C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

add_subdirectory(third_party/stb_image)

add_executable(img2txt)

target_sources(img2txt PRIVATE
    src/main.c
    src/args.c
    src/image.c
    src/image_transform.c
    src/text_art.c
    src/utf8.c
    src/util.c
)

if(WIN32)
    target_sources(img2txt PRIVATE
        src/win32.c
    )
endif()

if(MSVC)
    target_compile_definitions(img2txt PRIVATE
        _CRT_SECURE_NO_WARNINGS
    )
    target_compile_options(img2txt PRIVATE
        /utf-8
    )
endif()

if(UNIX)
    # For flockfile() and funlockfile()
    target_compile_definitions(img2txt PRIVATE
        _POSIX_C_SOURCE=200809L
    )

    # For math functions
    target_link_libraries(img2txt PRIVATE
        m
    )
endif()

target_link_libraries(img2txt PRIVATE
    stb_image
)
