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

static void ascii_art_init(struct ascii_art *art, int width, int height) {
    art->width = width;
    art->height = height;

    size_t buffer_size = (size_t)width * height + 1;

    art->buffer = malloc(buffer_size);
    art->buffer[buffer_size - 1] = '\0';
}

void ascii_art_create(
    struct ascii_art *art,
    const struct image *img,
    const struct ascii_art_config *config
) {
    const char *ascii_ramp = config->ramp ? config->ramp : ASCII_ART_RAMP_STANDARD;
    const size_t ramp_len = strlen(ascii_ramp);

    int out_width = config->out_width;
    int out_height = img->height * out_width / img->width;

    ascii_art_init(art, out_width, out_height);

    char *buffer_ptr = art->buffer;

    for (int y = 0; y < out_height; y++) {
        int src_y = y * img->width / out_width;

        for (int x = 0; x < out_width; x++) {
            int src_x = x * img->width / out_width;

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
    free(art->buffer);
    art->buffer = NULL;
}

void ascii_art_write(const struct ascii_art *art, FILE *stream) {
    const char *p = art->buffer;

    for (int y = 0; y < art->height; y++) {
        const char *end = p + art->width;

        while (p != end) {
            char c = *p++;
            putc(c, stream);
            putc(c, stream);
        }

        putc('\n', stream);
    }
}
