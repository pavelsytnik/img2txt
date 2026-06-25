/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Copyright (c) 2026 Pavlo Sytnyk.                                      *
 * Licensed under the MIT License. See LICENSE for license information.  *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "args.h"
#include "ascii_art.h"
#include "image.h"
#include "util.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEFAULT_OUT_WIDTH 60

struct arguments {
    const char *img_filename;
    double contrast;
    int width;
    bool use_weighted_grayscale;
};

static void arguments_init(struct arguments *args) {
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
static void parse_args(int argc, char const *const *argv, struct arguments *out_args) {
    arguments_init(out_args);

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

int main(int argc, char **argv) {
    struct arguments args;
    parse_args(argc, argv, &args);

    struct image img;
    image_load(args.img_filename, &img);

    char txt_art_filename[256];
    snprintf(txt_art_filename, sizeof(txt_art_filename), "%s.txt", args.img_filename);

    FILE *txt_art_file = fopen(txt_art_filename, "w");

    if (!txt_art_file) {
        image_free(&img);
        error("Failed to create file '%s'", txt_art_filename);
    }

    ascii_art_write(
        txt_art_file,
        &img,
        &(struct ascii_art_config) {
            .out_width = args.width,
            .contrast = args.contrast,
            .use_weighted_grayscale = args.use_weighted_grayscale
        }
    );

    fclose(txt_art_file);
    image_free(&img);

    return EXIT_SUCCESS;
}
