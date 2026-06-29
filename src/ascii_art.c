/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Copyright (c) 2026 Pavlo Sytnyk.                                      *
 * Licensed under the MIT License. See LICENSE for license information.  *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "ascii_art.h"

#include "image.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static uint8_t adjust_contrast(uint8_t v, double contrast) {
    double f = (v - 128.0) * contrast + 128.0;
    if (f < 0.0) f = 0.0;
    if (f > 255.0) f = 255.0;
    return (uint8_t)f;
}

static uint8_t pixel_to_grayscale(const uint8_t *pixel, int channels, bool weighted) {
    if (channels < 3) {
        return pixel[0];
    }
    if (weighted) {
        return (uint8_t)(0.299 * pixel[0] + 0.587 * pixel[1] + 0.114 * pixel[2]);
    }
    return (uint8_t)((pixel[0] + pixel[1] + pixel[2]) / 3);
}

void ascii_art_create(
    struct ascii_art *art,
    const struct image *img,
    const struct ascii_art_config *config
) {
    const char *ascii_ramp = "@%#*+=-:. ";
    const size_t ramp_len = strlen(ascii_ramp);

    int out_height = img->height * config->out_width / img->width;

    art->width = config->out_width;
    art->height = out_height;
    art->buffer = malloc(art->width * art->height + 1);

    art->buffer[art->width * art->height] = '\0';

    char *buffer_ptr = art->buffer;

    for (int y = 0; y < out_height; y++) {
        int src_y = y * img->width / config->out_width;

        for (int x = 0; x < config->out_width; x++) {
            int src_x = x * img->width / config->out_width;

            const uint8_t *px_ptr = image_pixel(img, src_x, src_y);

            uint8_t px_light = pixel_to_grayscale(
                px_ptr, img->channel_count, config->use_weighted_grayscale
            );

            px_light = adjust_contrast(px_light, config->contrast);

            char ascii_light = ascii_ramp[px_light * ramp_len / 256];

            *buffer_ptr++ = ascii_light;
        }
    }
}

void ascii_art_free(struct ascii_art *art) {
    if (art && art->buffer) {
        free(art->buffer);
    }
    art->buffer = NULL;
}

void ascii_art_write(const struct ascii_art *art, FILE *stream) {
    for (int y = 0; y < art->height; y++) {
        for (int x = 0; x < art->width; x++) {
            putc(art->buffer[y * art->width + x], stream);
            putc(art->buffer[y * art->width + x], stream);
        }
        putc('\n', stream);
    }
}
