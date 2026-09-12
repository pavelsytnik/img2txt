/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Copyright (c) 2026 Pavlo Sytnyk.                                      *
 * Licensed under the MIT License. See LICENSE for license information.  *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "util.h"

#ifdef _WIN32
  #include "win32.h"
#endif

#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdnoreturn.h>

noreturn void usage(void) {
    const char *usage_text =
        "Usage: img2txt [OPTIONS] <IMG_FILENAME>\n"
        "\n"
        "Options:\n"
        "  -w, --width=INT              Text art width in characters\n"
        "  -c, --contrast=DOUBLE        Text art contrast\n"
        "  -t, --terminal               Print art to the terminal\n"
        "  -o, --output=STRING          Output filename\n"
        "  -r, --ramp=STRING            Character ramp. Available values are 'ascii' and 'block'\n"
        "      --no-weighted-grayscale  Disable weighted grayscale\n"
        "      --no-box-filter          Disable box filtering\n";

    fprintf(stderr, "%s", usage_text);
    exit(EXIT_FAILURE);
}

noreturn void error(const char *fmt, ...) {
    assert(fmt != NULL);

    va_list v_args;
    va_start(v_args, fmt);

    fprintf(stderr, "Error: ");
    vfprintf(stderr, fmt, v_args);
    fprintf(stderr, "\n");

    va_end(v_args);

    exit(EXIT_FAILURE);
}

FILE *img2txt_fopen(const char *filename, const char *mode) {
#ifdef _WIN32
    return win32_fopen(filename, mode);
#else
    return fopen(filename, mode);
#endif
}
