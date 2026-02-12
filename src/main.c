/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Copyright (c) 2026 Pavlo Sytnyk.                                      *
 * Licensed under the MIT License. See LICENSE for license information.  *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <stb_image.h>

#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdnoreturn.h>
#include <string.h>

#define DEFAULT_OUT_WIDTH 60

struct args {
    const char *img_filename;
    int width;
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
    args->width = DEFAULT_OUT_WIDTH;
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
        const char *width_prefix = "--width=";
        size_t width_prefix_len = strlen(width_prefix);

        if (!strncmp(argv[i], width_prefix, width_prefix_len)) {
            int parsed_arg = atoi(argv[i] + width_prefix_len);

            if (parsed_arg <= 0) {
                error("Width value must be a positive integer\n");
            }

            out_args->width = parsed_arg;
            continue;
        }

        if (!out_args->img_filename) {
            out_args->img_filename = argv[i];
            continue;
        }

        error("Too many arguments provided\n");
        // The usage was previously displayed here
    }

    if (!out_args->img_filename) {
        error("Missing <FILENAME> argument\n");
        // The usage was previously displayed here
    }
}

void image_load(const char *filename, struct image *out) {
    out->data = stbi_load(filename, &out->width, &out->height, &out->channel_count, 0);

    if (!out->data) {
        error("File '%s' does not exist\n", filename);
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
        error("Failed to create file '%s'\n", txt_art_filename);
    }

    int out_width = args.width;
    int out_height = img.height * out_width / img.width;

    for (int y = 0; y < out_height; y++) {
        int src_y = y * img.width / out_width;

        for (int x = 0; x < out_width; x++) {
            int src_x = x * img.width / out_width;

            const stbi_uc *px_ptr = &img.data[(src_y * img.width + src_x) * img.channel_count];

            uint8_t px_light;
            if (img.channel_count < 3) {
                px_light = px_ptr[0];
            } else {
                px_light = (px_ptr[0] + px_ptr[1] + px_ptr[2]) / 3;
            }

            const char *ascii_ramp = "@%#*+=-:. ";
            char ascii_light = ascii_ramp[px_light * strlen(ascii_ramp) / 256];

            putc(ascii_light, txt_art_file);
            putc(ascii_light, txt_art_file);
        }

        putc('\n', txt_art_file);
    }

    fclose(txt_art_file);
    stbi_image_free(img.data);

    return EXIT_SUCCESS;
}
