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

struct ascii_art_context {
    const char *ramp;
    size_t ramp_len;
    double contrast;
    bool use_weighted_grayscale;
};

static void ascii_art_context_init(
    struct ascii_art_context *ctx,
    const struct ascii_art_config *config
) {
    ctx->ramp = (config->ramp)
        ? config->ramp
        : ASCII_ART_RAMP_STANDARD;

    ctx->ramp_len = strlen(ctx->ramp);

    ctx->contrast = config->contrast;
    ctx->use_weighted_grayscale = config->use_weighted_grayscale;
}

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

static uint8_t pixel_to_ascii(
    const uint8_t *pixel,
    int channels,
    const struct ascii_art_context *ctx
) {
    uint8_t px_light = pixel_to_grayscale(
        pixel, channels, ctx->use_weighted_grayscale
    );

    px_light = adjust_contrast(px_light, ctx->contrast);

    return ctx->ramp[px_light * ctx->ramp_len / 256];
}

static void ascii_art_init(struct ascii_art *art, int width, int height) {
    art->width = width;
    art->height = height;

    size_t buffer_size = (size_t)width * height + 1;

    art->buffer = malloc(buffer_size);
    art->buffer[buffer_size - 1] = '\0';
}

static void ascii_art_sample_image(
    const struct ascii_art *art,
    const struct image *img,
    const struct ascii_art_context *ctx
) {
    char *buffer = art->buffer;

    for (int y = 0; y < art->height; y++) {
        int img_y = y * img->height / art->height;

        for (int x = 0; x < art->width; x++) {
            int img_x = x * img->width / art->width;

            const uint8_t *pixel = image_pixel(img, img_x, img_y);
            char ascii = pixel_to_ascii(pixel, img->channel_count, ctx);

            *buffer++ = ascii;
        }
    }
}

void ascii_art_create(
    struct ascii_art *art,
    const struct image *img,
    const struct ascii_art_config *config
) {
    struct ascii_art_context ctx;

    int out_width = config->out_width;
    int out_height = img->height * out_width / img->width;

    ascii_art_init(art, out_width, out_height);
    ascii_art_context_init(&ctx, config);
    ascii_art_sample_image(art, img, &ctx);
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
