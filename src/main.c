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

struct arguments {
    const char *img_filename;
    const char *out_filename;
    const char *ramp;
    double contrast;
    int width;
    bool weighted_grayscale;
    bool terminal_output;
    bool box_filter;
};

static void arguments_init(struct arguments *args) {
    args->img_filename = NULL;
    args->out_filename = NULL;
    args->ramp = TEXT_ART_ASCII_RAMP_STANDARD;
    args->contrast = 1.0;
    args->width = 60;
    args->weighted_grayscale = true;
    args->terminal_output = false;
    args->box_filter = true;
}

static bool parse_opt(int key, const char *arg, struct args_state *state) {
    struct arguments *args = state->input;

    switch (key) {
        case 'w': {
            int parsed_arg = atoi(arg);

            if (parsed_arg <= 0) {
                fprintf(stderr, "Width value must be a positive integer\n");
                return false;
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
            args->out_filename = arg;
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
                fprintf(stderr, "Invalid --ramp value\n");
                return false;
            }
            break;
        }
        case KEY_NO_WEIGHTED_GRAYSCALE: {
            args->weighted_grayscale = false;
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
                fprintf(stderr, "Too many arguments provided\n");
                return false;
            }
            break;
        }
        case ARGS_KEY_INIT: {
            arguments_init(args);
            break;
        }
        case ARGS_KEY_END: {
            if (state->arg_num < 1) {
                fprintf(stderr, "<IMG_FILENAME> argument is missing\n");
                return false;
            }
            break;
        }
    }

    return true;
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
    if (!(argv = win32_argv_fetch())) {
        fprintf(stderr, "Command-line argument fetching failed\n");
        return EXIT_FAILURE;
    }

    win32_console_enable_utf8();
#endif

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

    struct image img;
    struct text_art art;
    char *art_filename;

    bool ok;

    if (argc < 2) {
        usage(); // It exits execution
    }

    ok = args_parse(&program, argc, argv, ARGS_NO_EXIT);
    if (!ok) {
        goto end;
    }

    ok = image_load(&img, args.img_filename);
    if (!ok) {
        fprintf(stderr, "Image '%s' loading failed\n", args.img_filename);
        goto end;
    }

    ok = text_art_create(
        &art,
        &img,
        &(struct text_art_config) {
            .ramp = args.ramp,
            .width = args.width,
            .contrast = args.contrast,
            .weighted_grayscale = args.weighted_grayscale,
            .box_filter = args.box_filter
        }
    );
    if (!ok) {
        fprintf(stderr, "Text art creation failed\n");
        goto img_cleanup;
    }

    art_filename = build_output_filename(
        args.out_filename,
        args.img_filename
    );

    // The following code preserves any previous error.

    if (art_filename) {
        if (!text_art_save(&art, art_filename)) {
            ok = false;
            fprintf(stderr, "Saving text art to '%s' failed\n", art_filename);
        }
    } else {
        ok = false;
        fprintf(stderr, "Output filename building failed\n");
    }

    if (args.terminal_output) {
        if (!text_art_write(&art, stdout)) {
            ok = false;
            fprintf(stderr, "Text art output to stdout failed\n");
        }
    }

    free(art_filename);
    text_art_destroy(&art);
img_cleanup:
    image_destroy(&img);

end:

#ifdef _WIN32
    win32_argv_free(argv);
#endif

    return (ok) ? EXIT_SUCCESS : EXIT_FAILURE;
}
