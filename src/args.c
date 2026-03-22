/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Copyright (c) 2026 Pavlo Sytnyk.                                      *
 * Licensed under the MIT License. See LICENSE for license information.  *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "args.h"

#include "util.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#define DEFAULT_OUT_WIDTH 60

static void args_init(struct args *args) {
    args->img_filename = NULL;
    args->contrast = 1.0;
    args->width = DEFAULT_OUT_WIDTH;
    args->use_weighted_grayscale = false;
}

/*
    Now this function successfully parses cases where the options comes
    before or after the <FILENAME> argument.

    In the future, it is worth considering an order where the options come
    strictly at the front.
*/
void parse_args(int argc, char const *const *argv, struct args *out_args) {
    args_init(out_args);

    if (argc < 2) {
        usage();
    }

    for (int i = 1; i < argc; i++) {
        if (!strncmp(argv[i], "--width=", 8)) {
            int parsed_arg = atoi(argv[i] + 8);

            if (parsed_arg <= 0) {
                error("Width value must be a positive integer");
            }

            out_args->width = parsed_arg;
            continue;
        }

        if (!strncmp(argv[i], "--contrast=", 11)) {
            double parsed_arg = atof(argv[i] + 11);

            out_args->contrast = parsed_arg;
            continue;
        }

        if (!strcmp(argv[i], "--weighted-grayscale")) {
            out_args->use_weighted_grayscale = true;
            continue;
        }

        if (!out_args->img_filename) {
            out_args->img_filename = argv[i];
            continue;
        }

        error("Too many arguments provided");
        // The usage was previously displayed here
    }

    if (!out_args->img_filename) {
        error("Missing <FILENAME> argument");
        // The usage was previously displayed here
    }
}
