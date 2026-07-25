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

#define KEY_WEIGHTED_GRAYSCALE 256

#define DEFAULT_OUT_WIDTH 60

struct arguments {
    const char *img_filename;
    double contrast;
    int width;
    bool use_weighted_grayscale;
    bool terminal_output;
};

static void arguments_init(struct arguments *args) {
    args->img_filename = NULL;
    args->contrast = 1.0;
    args->width = DEFAULT_OUT_WIDTH;
    args->use_weighted_grayscale = false;
    args->terminal_output = false;
}

static void parse_opt(int key, const char *arg, void *data) {
    struct arguments *args = data;

    switch (key) {
        case 'w': {
            int parsed_arg = atoi(arg);

            if (parsed_arg <= 0) {
                error("Width value must be a positive integer");
            }

            args->width = parsed_arg;
            break;
        }
        case 'c': {
            double parsed_arg = atof(arg);

            args->contrast = parsed_arg;
            break;
        }
        case 't': {
            args->terminal_output = true;
            break;
        }
        case KEY_WEIGHTED_GRAYSCALE: {
            args->use_weighted_grayscale = true;
            break;
        }
        case ARGS_KEY_ARG: {
            if (!args->img_filename) {
                args->img_filename = arg;
                break;
            }

            error("Too many arguments provided");
            break;
        }
        case ARGS_KEY_INIT: {
            arguments_init(args);
            break;
        }
        case ARGS_KEY_END: {
            if (!args->img_filename) {
                error("Missing <FILENAME> argument");
            }
            break;
        }
    }
}

int main(int argc, char **argv) {
#ifdef ARGS_PLATFORM_WINDOWS
    args_windows_args_fetch(&argc, &argv);
#endif

    if (argc < 2) {
        usage();
    }

    struct args_option options[] = {
        { 'w', "width", "INT" },
        { 'c', "contrast", "DOUBLE" },
        { 't', "terminal", 0 },
        { KEY_WEIGHTED_GRAYSCALE, "weighted-grayscale", 0 },
        { 0 }
    };

    struct arguments args;

    struct args_program program = { options, parse_opt, &args };
    args_parse(&program, argc, argv);

    struct image img;
    image_load(args.img_filename, &img);

    struct ascii_art art;
    ascii_art_create(
        &art,
        &img,
        &(struct ascii_art_config) {
            .ramp = ASCII_ART_RAMP_STANDARD,
            .out_width = args.width,
            .contrast = args.contrast,
            .use_weighted_grayscale = args.use_weighted_grayscale
        }
    );

    char txt_art_filename[256];
    snprintf(txt_art_filename, sizeof(txt_art_filename), "%s.txt", args.img_filename);

    FILE *txt_art_file = fopen(txt_art_filename, "w");

    if (!txt_art_file) {
        image_free(&img);
        error("Failed to create file '%s'", txt_art_filename);
    }

    ascii_art_write(&art, txt_art_file);

    if (args.terminal_output) {
        ascii_art_write(&art, stdout);
    }

    fclose(txt_art_file);
    ascii_art_free(&art);
    image_free(&img);

#ifdef ARGS_PLATFORM_WINDOWS
    args_windows_args_free(&argc, &argv);
#endif

    return EXIT_SUCCESS;
}
