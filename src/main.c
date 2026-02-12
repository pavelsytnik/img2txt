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

void usage(const char *prog_name) {
    fprintf(stderr, "Usage: %s [--width=INT] <FILENAME>\n", prog_name);
}

int main(int argc, char **argv) {
    if (argc < 2) {
        usage(argv[0]);
        return EXIT_FAILURE;
    }

    size_t arg_idx = 1;

    int opt_width = DEFAULT_OUT_WIDTH;

    const char *width_prefix = "--width=";
    size_t width_prefix_len = strlen(width_prefix);

    if (!strncmp(argv[arg_idx], width_prefix, width_prefix_len)) {
        int parsed_arg = atoi(argv[arg_idx] + width_prefix_len);

        if (parsed_arg <= 0) {
            fprintf(stderr, "Error: width value must be a positive integer\n");
            return EXIT_FAILURE;
        }

        opt_width = parsed_arg;
        arg_idx++;
    }

    const char *img_filename = argv[arg_idx];

    if (!img_filename) {
        fprintf(stderr, "Error: missing <FILENAME> argument\n");
        usage(argv[0]);
        return EXIT_FAILURE;
    }

    if (argv[++arg_idx]) {
        fprintf(stderr, "Error: too many arguments provided\n");
        usage(argv[0]);
        return EXIT_FAILURE;
    }

    int width;
    int height;
    int channel_count;

    stbi_uc *img_data = stbi_load(img_filename, &width, &height, &channel_count, 0);

    if (!img_data) {
        fprintf(stderr, "Error: file '%s' does not exist\n", img_filename);
        return EXIT_FAILURE;
    }

    char txt_art_filename[256];
    snprintf(txt_art_filename, sizeof(txt_art_filename), "%s.txt", img_filename);

    FILE *txt_art_file = fopen(txt_art_filename, "w");

    if (!txt_art_file) {
        fprintf(stderr, "Error: failed to create file '%s'\n", txt_art_filename);
        stbi_image_free(img_data);
        return EXIT_FAILURE;
    }

    int out_width = opt_width;
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
