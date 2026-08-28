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

struct utf8_char_view;

struct text_art_context {
    struct utf8_char_view *ramp;
    size_t ramp_len;
    double contrast;
    bool use_weighted_grayscale;
    enum image_resize_filter filter;
};

struct utf8_char_view {
    const char *data;
    size_t size;
};

static size_t utf8_length(const char *s) {
    size_t n = 0;

    while (*s) {
        if ((*s & 0xC0) != 0x80) n++;
        s++;
    }

    return n;
}

static size_t utf8_char_length(const char *s) {
    uint8_t c = (uint8_t)*s;

    if ((c & 0x80) == 0x00) return 1;
    if ((c & 0xE0) == 0xC0) return 2;
    if ((c & 0xF0) == 0xE0) return 3;
    if ((c & 0xF8) == 0xF0) return 4;

    return 0;
}

static size_t utf8_longest_char_length(const char *s) {
    size_t max = 0;

    while (*s) {
        size_t len = utf8_char_length(s);

        if (len == 0 || len == 4) return len;

        if (len > max) {
            max = len;
        }
        s += len;
    }

    return max;
}

static void text_art_context_init(
    struct text_art_context *ctx,
    const struct text_art_config *config
) {
    const char *ramp = (config->ramp)
        ? config->ramp
        : TEXT_ART_ASCII_RAMP_STANDARD;

    ctx->ramp_len = utf8_length(ramp);

    ctx->ramp = malloc(ctx->ramp_len * sizeof(struct utf8_char_view));

    const char *ramp_char = ramp;
    for (size_t i = 0; i < ctx->ramp_len; i++) {
        ctx->ramp[i].data = ramp_char;
        ctx->ramp[i].size = utf8_char_length(ramp_char);

        ramp_char += ctx->ramp[i].size;
    }

    ctx->contrast = config->contrast;
    ctx->use_weighted_grayscale = config->use_weighted_grayscale;
    ctx->filter = (config->box_filter)
        ? IMAGE_RESIZE_FILTER_BOX
        : IMAGE_RESIZE_FILTER_NEAREST_NEIGHBOR;
}

static void text_art_context_destroy(struct text_art_context *ctx) {
    free(ctx->ramp);
    memset(ctx, 0, sizeof(struct text_art_context));
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

static const struct utf8_char_view *pixel_to_glyph(
    const uint8_t *pixel,
    int channels,
    const struct text_art_context *ctx
) {
    uint8_t px_light = pixel_to_grayscale(
        pixel, channels, ctx->use_weighted_grayscale
    );

    px_light = adjust_contrast(px_light, ctx->contrast);

    return &ctx->ramp[px_light * ctx->ramp_len / 256];
}

static void text_art_init(struct text_art *art, int width, int height, size_t max_glyph_bytes) {
    art->width = width;
    art->height = height;

    size_t buffer_size = (size_t)width * height * max_glyph_bytes + 1;

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
            const struct utf8_char_view *glyph = pixel_to_glyph(pixel, img->channel_count, ctx);

            memcpy(buffer, glyph->data, glyph->size);
            buffer += glyph->size;
        }
    }
}

static void text_art_populate(
    struct text_art *art,
    const struct image *img,
    const struct text_art_context *ctx
) {
    struct image resized_img;
    image_resized(&resized_img, img, art->width, art->height, ctx->filter);

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

    size_t longest_glyph_size = utf8_longest_char_length(
        (config->ramp) ? config->ramp : TEXT_ART_ASCII_RAMP_STANDARD
    );

    text_art_init(art, out_width, out_height, longest_glyph_size);
    text_art_context_init(&ctx, config);

    text_art_populate(art, img, &ctx);

    text_art_context_destroy(&ctx);
}

void text_art_destroy(struct text_art *art) {
    free(art->buffer);
    memset(art, 0, sizeof(struct text_art));
}

void text_art_write(const struct text_art *art, FILE *stream) {
    const char *p = art->buffer;

    for (int y = 0; y < art->height; y++) {
        for (int x = 0; x < art->width; x++) {
            size_t n = utf8_char_length(p);
            fwrite(p, 1, n, stream);
            p += n;
        }
        putc('\n', stream);
    }
}
