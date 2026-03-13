/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Copyright (c) 2026 Pavlo Sytnyk.                                      *
 * Licensed under the MIT License. See LICENSE for license information.  *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "ascii_art.h"

#include "image.h"

#include <stb_image.h>

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

static uint8_t adjust_contrast(uint8_t v, double contrast) {
    double f = (v - 128.0) * contrast + 128.0;
    if (f < 0.0) f = 0.0;
    if (f > 255.0) f = 255.0;
    return (uint8_t)f;
}

void write_ascii_art(
    FILE *dst,
    const struct image *img,
    int out_width,
    double contrast,
    bool use_weighted_grayscale
) {
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
                    px_light = (uint8_t)((px_ptr[0] + px_ptr[1] + px_ptr[2]) / 3);
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
