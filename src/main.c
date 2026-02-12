/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Copyright (c) 2026 Pavlo Sytnyk.                                      *
 * Licensed under the MIT License. See LICENSE for license information.  *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <stb_image.h>

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEFAULT_OUT_WIDTH 60

struct args {
    const char *img_filename;
    int width;
};

void usage(const char *prog_name) {
    fprintf(stderr, "Usage: %s [--width=INT] <FILENAME>\n", prog_name);
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
        exit(EXIT_FAILURE);
    }

    for (int i = 1; i < argc; i++) {
        const char *width_prefix = "--width=";
        size_t width_prefix_len = strlen(width_prefix);

        if (!strncmp(argv[i], width_prefix, width_prefix_len)) {
            int parsed_arg = atoi(argv[i] + width_prefix_len);

            if (parsed_arg <= 0) {
                fprintf(stderr, "Error: width value must be a positive integer\n");
                exit(EXIT_FAILURE);
            }

            out_args->width = parsed_arg;
            continue;
        }

        if (!out_args->img_filename) {
            out_args->img_filename = argv[i];
            continue;
        }

        fprintf(stderr, "Error: too many arguments provided\n");
        usage(argv[0]);
        exit(EXIT_FAILURE);
    }

    if (!out_args->img_filename) {
        fprintf(stderr, "Error: missing <FILENAME> argument\n");
        usage(argv[0]);
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char **argv) {
    struct args args;
    parse_args(argc, argv, &args);

    int width;
    int height;
    int channel_count;

    stbi_uc *img_data = stbi_load(args.img_filename, &width, &height, &channel_count, 0);

    if (!img_data) {
        fprintf(stderr, "Error: file '%s' does not exist\n", args.img_filename);
        return EXIT_FAILURE;
    }

    char txt_art_filename[256];
    snprintf(txt_art_filename, sizeof(txt_art_filename), "%s.txt", args.img_filename);

    FILE *txt_art_file = fopen(txt_art_filename, "w");

    if (!txt_art_file) {
        fprintf(stderr, "Error: failed to create file '%s'\n", txt_art_filename);
        stbi_image_free(img_data);
        return EXIT_FAILURE;
    }

    int out_width = args.width;
    int out_height = height * out_width / width;

    for (int y = 0; y < out_height; y++) {
        int src_y = y * width / out_width;

        for (int x = 0; x < out_width; x++) {
            int src_x = x * width / out_width;

            const stbi_uc *px_ptr = &img_data[(src_y * width + src_x) * channel_count];

            uint8_t px_light;
            if (channel_count < 3) {
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
    stbi_image_free(img_data);

    return EXIT_SUCCESS;
}
