/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Copyright (c) 2026 Pavlo Sytnyk.                                      *
 * Licensed under the MIT License. See LICENSE for license information.  *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "text_art.h"

#include "image.h"
#include "image_transform.h"
#include "utf8.h"
#include "util.h"

#include <assert.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct text_art_renderer;

struct text_art_context {
    const struct text_art_renderer *renderer;
    struct utf8_char_view *ramp;
    size_t ramp_len;
    double contrast;
    bool use_weighted_grayscale;
    enum image_resize_filter filter;
};

struct text_art_renderer {
    void (*image_size)(
        int art_width,
        int art_height,
        int *img_width,
        int *img_height
    );
    void (*render)(
        char *buffer,
        const struct image *img,
        const struct text_art_context *ctx
    );
};

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

static void image_size_ramp(
    int art_width,
    int art_height,
    int *img_width,
    int *img_height
) {
    *img_width = art_width;
    *img_height = art_height;
}

static void render_ramp(
    char *buffer,
    const struct image *img,
    const struct text_art_context *ctx
);

static const struct text_art_renderer *get_renderer(enum text_art_mode mode) {
    static const struct text_art_renderer ramp_renderer = {
        .image_size = image_size_ramp,
        .render = render_ramp
    };

    switch (mode) {
    case TEXT_ART_MODE_RAMP:
        return &ramp_renderer;
    default:
        return NULL;
    }
}

static bool text_art_context_init(
    struct text_art_context *ctx,
    const struct text_art_config *config
) {
    const char *raw_ramp = (config->ramp)
        ? config->ramp
        : TEXT_ART_ASCII_RAMP_STANDARD;

    size_t ramp_len = utf8_length(raw_ramp);

    // There's only one renderer for now
    const struct text_art_renderer *renderer = get_renderer(TEXT_ART_MODE_RAMP);
    if (!renderer) return false;

    struct utf8_char_view *ramp = utf8_char_views(raw_ramp);
    if (!ramp) return false;

    ctx->renderer = renderer;
    ctx->ramp = ramp;
    ctx->ramp_len = ramp_len;
    ctx->contrast = config->contrast;
    ctx->use_weighted_grayscale = config->use_weighted_grayscale;
    ctx->filter = (config->box_filter)
        ? IMAGE_RESIZE_FILTER_BOX
        : IMAGE_RESIZE_FILTER_NEAREST_NEIGHBOR;

    return true;
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

static bool text_art_init(struct text_art *art, int width, int height, size_t max_glyph_bytes) {
    size_t buffer_size = (size_t)width * height * max_glyph_bytes + 1;

    char *buffer = malloc(buffer_size);
    if (!buffer) return false;

    buffer[buffer_size - 1] = '\0';

    art->width = width;
    art->height = height;
    art->buffer = buffer;

    return true;
}

static void render_ramp(
    char *buffer,
    const struct image *img,
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

static bool text_art_populate(
    struct text_art *art,
    const struct image *img,
    const struct text_art_context *ctx
) {
    struct image resized_img;
    int width, height;

    ctx->renderer->image_size(art->width, art->height, &width, &height);

    if (!image_resized(&resized_img, img, width, height, ctx->filter)) {
        return false;
    }

    ctx->renderer->render(art->buffer, &resized_img, ctx);

    image_destroy(&resized_img);

    return true;
}

bool text_art_create(
    struct text_art *art,
    const struct image *img,
    const struct text_art_config *config
) {
    assert(art != NULL);
    assert(img != NULL);
    assert(img->width > 0);
    assert(img->height > 0);
    assert(img->channel_count >= 1 && img->channel_count <= 4);
    assert(img->data != NULL);
    assert(config != NULL);
    assert(config->out_width > 0);

    struct text_art_context ctx;

    int out_width = config->out_width;
    int out_height = (int)round(img->height * out_width / (img->width * 2.0));

    size_t longest_glyph_size = utf8_longest_char_length(
        (config->ramp) ? config->ramp : TEXT_ART_ASCII_RAMP_STANDARD
    );
    assert(longest_glyph_size != 0);

    bool ok;

    ok = text_art_init(art, out_width, out_height, longest_glyph_size);
    if (!ok) goto end;

    ok = text_art_context_init(&ctx, config);
    if (!ok) goto cleanup_art;

    ok = text_art_populate(art, img, &ctx);

    text_art_context_destroy(&ctx);
cleanup_art:
    if (!ok) text_art_destroy(art);
end:
    return ok;
}

void text_art_destroy(struct text_art *art) {
    assert(art != NULL);

    free(art->buffer);
    memset(art, 0, sizeof(struct text_art));
}

bool text_art_write(const struct text_art *art, FILE *stream) {
    assert(art != NULL);
    assert(art->buffer != NULL);
    assert(stream != NULL);

    const char *p = art->buffer;

    for (int y = 0; y < art->height; y++) {
        for (int x = 0; x < art->width; x++) {
            size_t n = utf8_char_length(p);

            if (fwrite(p, 1, n, stream) != n) {
                return false;
            }

            p += n;
        }

        if (putc('\n', stream) == EOF) {
            return false;
        }
    }

    return true;
}

bool text_art_save(const struct text_art *art, const char *filename) {
    FILE *file = img2txt_fopen(filename, "w");
    if (!file) return false;

    bool ok = text_art_write(art, file);

    if (fclose(file) != 0) {
        ok = false;
    }

    return ok;
}
