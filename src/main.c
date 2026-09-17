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

static void build_output_filename(
    char *buf,
    size_t bufsz,
    const char *out_filename,
    const char *img_filename
) {
    int n;

    if (out_filename) {
        n = snprintf(buf, bufsz, "%s", out_filename);
    } else if (img_filename) {
        n = snprintf(buf, bufsz, "%s.txt", img_filename);
    } else {
        n = snprintf(buf, bufsz, "img2txt_generated.txt");
    }

    if (n == 0) {
        error("Given path is empty");
    } else if (n >= bufsz) {
        error("Given path is too long");
    }
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
    char txt_art_filename[256];
    FILE *txt_art_file;

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

    build_output_filename(
        txt_art_filename,
        sizeof(txt_art_filename),
        args.output_filename,
        args.img_filename
    );

    txt_art_file = img2txt_fopen(txt_art_filename, "w");
    if (!txt_art_file) {
        error("File '%s' creation failed", txt_art_filename);
    }

    if (!text_art_write(&art, txt_art_file)) {
        error("Text art output to '%s' failed", txt_art_filename);
    }

    if (args.terminal_output) {
        if (!text_art_write(&art, stdout)) {
            error("Text art output to stdout failed");
        }
    }

    fclose(txt_art_file);
    text_art_destroy(&art);
    image_destroy(&img);

#ifdef _WIN32
    win32_argv_free(argv);
#endif

    return EXIT_SUCCESS;
}
