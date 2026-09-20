/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Copyright (c) 2026 Pavlo Sytnyk.                                      *
 * Licensed under the MIT License. See LICENSE for license information.  *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "args.h"
#include "image.h"
#include "text_art.h"
#include "util.h"

#ifdef _WIN32
  #include "win32.h"
#endif

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define KEY_NO_WEIGHTED_GRAYSCALE 256
#define KEY_NO_BOX_FILTER 257

#define DEFAULT_OUT_WIDTH 60

struct arguments {
    const char *img_filename;
    const char *output_filename;
    const char *ramp;
    double contrast;
    int width;
    bool use_weighted_grayscale;
    bool terminal_output;
    bool box_filter;
};

static void arguments_init(struct arguments *args) {
    args->img_filename = NULL;
    args->output_filename = NULL;
    args->ramp = TEXT_ART_ASCII_RAMP_STANDARD;
    args->contrast = 1.0;
    args->width = DEFAULT_OUT_WIDTH;
    args->use_weighted_grayscale = true;
    args->terminal_output = false;
    args->box_filter = true;
}

static void parse_opt(int key, const char *arg, struct args_state *state) {
    struct arguments *args = state->data;

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
        case 'o': {
            args->output_filename = arg;
            break;
        }
        case 't': {
            args->terminal_output = true;
            break;
        }
        case 'r': {
            if (!strcmp(arg, "ascii")) {
                args->ramp = TEXT_ART_ASCII_RAMP_STANDARD;
            } else if (!strcmp(arg, "block")) {
                args->ramp = TEXT_ART_BLOCK_RAMP;
            } else {
                error("Invalid --ramp value");
            }
            break;
        }
        case KEY_NO_WEIGHTED_GRAYSCALE: {
            args->use_weighted_grayscale = false;
            break;
        }
        case KEY_NO_BOX_FILTER: {
            args->box_filter = false;
            break;
        }
        case ARGS_KEY_ARG: {
            if (state->arg_num == 0) {
                args->img_filename = arg;
            } else {
                error("Too many arguments provided");
            }
            break;
        }
        case ARGS_KEY_INIT: {
            arguments_init(args);
            break;
        }
        case ARGS_KEY_END: {
            if (state->arg_num < 1) {
                error("<IMG_FILENAME> argument is missing");
            }
            break;
        }
    }
}

static char *build_output_filename(
    const char *out_filename,
    const char *img_filename
) {
    const char *format;
    const char *filename;

    if (out_filename) {
        format = "%s";
        filename = out_filename;
    } else if (img_filename) {
        format = "%s.txt";
        filename = img_filename;
    } else {
        return NULL;
    }

    int n = snprintf(NULL, 0, format, filename);
    if (n < 0) return NULL;

    char *buf = malloc((size_t)n + 1);
    if (!buf) return NULL;

    if (snprintf(buf, (size_t)n + 1, format, filename) < 0) {
        free(buf);
        return NULL;
    }

    return buf;
}

int main(int argc, char **argv) {
#ifdef _WIN32
    argv = win32_argv_fetch();
    win32_console_enable_utf8();
#endif

    if (argc < 2) {
        usage();
    }

    struct args_option options[] = {
        { 'w', "width", "INT" },
        { 'c', "contrast", "DOUBLE" },
        { 't', "terminal", 0 },
        { 'o', "output", "STRING" },
        { 'r', "ramp", "STRING" },
        { KEY_NO_WEIGHTED_GRAYSCALE, "no-weighted-grayscale", 0 },
        { KEY_NO_BOX_FILTER, "no-box-filter", 0 },
        { 0 }
    };

    struct arguments args;

    struct args_program program = { options, parse_opt, &args };
    args_parse(&program, argc, argv); // This function aborts execution on error

    struct image img;
    struct text_art art;
    char *txt_art_filename;

    if (!image_load(&img, args.img_filename)) {
        error("Image '%s' loading failed", args.img_filename);
    }

    if (!text_art_create(
        &art,
        &img,
        &(struct text_art_config) {
            .ramp = args.ramp,
            .out_width = args.width,
            .contrast = args.contrast,
            .use_weighted_grayscale = args.use_weighted_grayscale,
            .box_filter = args.box_filter
        }
    )) {
        error("Text art creation failed");
    }

    txt_art_filename = build_output_filename(
        args.output_filename,
        args.img_filename
    );
    if (!txt_art_filename) {
        error("Output filename building failed");
    }

    if (!text_art_save(&art, txt_art_filename)) {
        error("Saving text art to '%s' failed", txt_art_filename);
    }

    if (args.terminal_output) {
        if (!text_art_write(&art, stdout)) {
            error("Text art output to stdout failed");
        }
    }

    free(txt_art_filename);
    text_art_destroy(&art);
    image_destroy(&img);

#ifdef _WIN32
    win32_argv_free(argv);
#endif

    return EXIT_SUCCESS;
}
