/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Copyright (c) 2026 Pavlo Sytnyk.                                      *
 * Licensed under the MIT License. See LICENSE for license information.  *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "util.h"

#ifdef _WIN32
#  include <windows.h>
#  include <wchar.h>
#endif

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdnoreturn.h>

noreturn void usage(void) {
    const char *usage_text =
        "Usage: img2txt [--width=INT] [--contrast=DOUBLE] [--terminal] [--weighted-grayscale] <FILENAME>";
    fprintf(stderr, "%s\n", usage_text);
    exit(EXIT_FAILURE);
}

noreturn void error(const char *fmt, ...) {
    va_list v_args;
    va_start(v_args, fmt);

    fprintf(stderr, "Error: ");
    vfprintf(stderr, fmt, v_args);
    fprintf(stderr, "\n");

    va_end(v_args);

    exit(EXIT_FAILURE);
}

#ifdef _WIN32

static wchar_t *utf8_to_wide(const char *str) {
    int len = MultiByteToWideChar(CP_UTF8, 0, str, -1, NULL, 0);

    wchar_t *wstr = malloc(sizeof(wchar_t) * len);

    MultiByteToWideChar(CP_UTF8, 0, str, -1, wstr, len);

    return wstr;
}

static FILE *img2txt_windows_fopen(const char *filename, const char *mode) {
    wchar_t *wfilename = utf8_to_wide(filename);
    wchar_t *wmode = utf8_to_wide(mode);

    FILE *file = _wfopen(wfilename, wmode);

    free(wmode);
    free(wfilename);

    return file;
}

#endif // _WIN32

FILE *img2txt_fopen(const char *filename, const char *mode) {
#ifdef _WIN32
    return img2txt_windows_fopen(filename, mode);
#else
    return fopen(filename, mode);
#endif
}
