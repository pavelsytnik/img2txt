/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Copyright (c) 2026 Pavlo Sytnyk.                                      *
 * Licensed under the MIT License. See LICENSE for license information.  *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "util.h"

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
