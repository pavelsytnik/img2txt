/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Copyright (c) 2026 Pavlo Sytnyk.                                      *
 * Licensed under the MIT License. See LICENSE for license information.  *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <stb_image.h>

#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdnoreturn.h>
#include <string.h>

#define DEFAULT_OUT_WIDTH 60

struct args {
    const char *img_filename;
    double contrast;
    int width;
    bool use_weighted_grayscale;
};

struct image {
    int width;
    int height;
    int channel_count;
    stbi_uc *data;
};

noreturn void usage(const char *prog_name) {
    fprintf(stderr, "Usage: %s [--width=INT] <FILENAME>\n", prog_name);
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

void args_init(struct args *args) {
    args->img_filename = NULL;
    args->contrast = 1.0;
    args->width = DEFAULT_OUT_WIDTH;
    args->use_weighted_grayscale = false;
}

/*
    Now this function successfully parses cases where the --width option comes
    before or after the <FILENAME> argument.

    In the future, it is worth considering an order where the options come
    strictly at the front.
*/
void parse_args(int argc, char const *const *argv, struct args *out_args) {
    args_init(out_args);

    if (argc < 2) {
        usage(argv[0]);
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

void image_load(const char *filename, struct image *out) {
    out->data = stbi_load(filename, &out->width, &out->height, &out->channel_count, 0);

    if (!out->data) {
        error("File '%s' does not exist", filename);
    }
}

uint8_t adjust_contrast(uint8_t v, double contrast) {
    double f = (v - 128.0) * contrast + 128.0;
    if (f < 0.0) f = 0.0;
    if (f > 255.0) f = 255.0;
    return (uint8_t)f;
}

void write_ascii_art(FILE *dst, const struct image *img, int out_width, double contrast, bool use_weighted_grayscale) {
    int out_height = img->height * out_width / img->width;

    for (int y = 0; y < out_height; y++) {
        int src_y = y * img->width / out_width;

        for (int x = 0; x < out_width; x++) {
            int src_x = x * img->width / out_width;

            const stbi_uc *px_ptr = &img->data[(src_y * img->width + src_x) * img->channel_count];

            uint8_t px_light;
            if (img->channel_count < 3) {
                px_light = px_ptr[0];
            } else {
                if (use_weighted_grayscale) {
                    px_light = (uint8_t)(0.299 * px_ptr[0] + 0.587 * px_ptr[1] + 0.114 * px_ptr[2]);
                } else {
                    px_light = (px_ptr[0] + px_ptr[1] + px_ptr[2]) / 3;
                }
            }

            px_light = adjust_contrast(px_light, contrast);

            const char *ascii_ramp = "@%#*+=-:. ";
            char ascii_light = ascii_ramp[px_light * strlen(ascii_ramp) / 256];

            putc(ascii_light, dst);
            putc(ascii_light, dst);
        }

        putc('\n', dst);
    }
}

int main(int argc, char **argv) {
    struct args args;
    parse_args(argc, argv, &args);

    struct image img;
    image_load(args.img_filename, &img);

    char txt_art_filename[256];
    snprintf(txt_art_filename, sizeof(txt_art_filename), "%s.txt", args.img_filename);

    FILE *txt_art_file = fopen(txt_art_filename, "w");

    if (!txt_art_file) {
        stbi_image_free(img.data);
        error("Failed to create file '%s'", txt_art_filename);
    }

    write_ascii_art(txt_art_file, &img, args.width, args.contrast, args.use_weighted_grayscale);

    fclose(txt_art_file);
    stbi_image_free(img.data);

    return EXIT_SUCCESS;
}
