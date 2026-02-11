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

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <FILENAME>\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *img_filename = argv[1];

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

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            const stbi_uc *px_ptr = &img_data[(y * width + x) * channel_count];

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
