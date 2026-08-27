/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Copyright (c) 2026 Pavlo Sytnyk.                                      *
 * Licensed under the MIT License. See LICENSE for license information.  *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "text_art.h"

#include "image.h"
#include "image_transform.h"

#include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct text_art_context {
    const char *ramp;
    size_t ramp_len;
    double contrast;
    bool use_weighted_grayscale;
    enum image_resize_filter filter;
};

static void text_art_context_init(
    struct text_art_context *ctx,
    const struct text_art_config *config
) {
    ctx->ramp = (config->ramp)
        ? config->ramp
        : TEXT_ART_ASCII_RAMP_STANDARD;

    ctx->ramp_len = strlen(ctx->ramp);

    ctx->contrast = config->contrast;
    ctx->use_weighted_grayscale = config->use_weighted_grayscale;
    ctx->filter = (config->box_filter)
        ? IMAGE_RESIZE_FILTER_BOX
        : IMAGE_RESIZE_FILTER_NEAREST_NEIGHBOR;
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

static uint8_t pixel_to_glyph(
    const uint8_t *pixel,
    int channels,
    const struct text_art_context *ctx
) {
    uint8_t px_light = pixel_to_grayscale(
        pixel, channels, ctx->use_weighted_grayscale
    );

    px_light = adjust_contrast(px_light, ctx->contrast);

    return ctx->ramp[px_light * ctx->ramp_len / 256];
}

static void text_art_init(struct text_art *art, int width, int height) {
    art->width = width;
    art->height = height;

    size_t buffer_size = (size_t)width * height + 1;

    art->buffer = malloc(buffer_size);
    art->buffer[buffer_size - 1] = '\0';
}

static void sample_image(
    const struct image *img,
    char *buffer,
    const struct text_art_context *ctx
) {
    for (int y = 0; y < img->height; y++) {
        for (int x = 0; x < img->width; x++) {
            const uint8_t *pixel = image_pixel(img, x, y);
            char glyph = pixel_to_glyph(pixel, img->channel_count, ctx);

            *buffer++ = glyph;
        }
    }
}

static void text_art_populate(
    struct text_art *art,
    const struct image *img,
    const struct text_art_context *ctx
) {
    struct image resized_img;
    image_resize(img, &resized_img, art->width, art->height, ctx->filter);

    sample_image(&resized_img, art->buffer, ctx);

    image_destroy(&resized_img);
}

void text_art_create(
    struct text_art *art,
    const struct image *img,
    const struct text_art_config *config
) {
    struct text_art_context ctx;

    int out_width = config->out_width;
    int out_height = (int)round(img->height * out_width / (img->width * 2.0));

    text_art_init(art, out_width, out_height);
    text_art_context_init(&ctx, config);

    text_art_populate(art, img, &ctx);
}

void text_art_destroy(struct text_art *art) {
    free(art->buffer);
    memset(art, 0, sizeof(struct text_art));
}

void text_art_write(const struct text_art *art, FILE *stream) {
    const char *p = art->buffer;

    for (int y = 0; y < art->height; y++) {
        const char *end = p + art->width;

        while (p != end) {
            char c = *p++;
            putc(c, stream);
        }

        putc('\n', stream);
    }
}
