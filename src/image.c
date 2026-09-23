/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Copyright (c) 2026 Pavlo Sytnyk.                                      *
 * Licensed under the MIT License. See LICENSE for license information.  *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "image.h"

#include <stb_image.h>

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

bool image_init(struct image *img, int width, int height, int channels) {
    uint8_t *data = malloc(width * height * channels);
    if (!data) return false;

    img->width = width;
    img->height = height;
    img->channel_count = channels;
    img->data = data;

    return true;
}

bool image_load(struct image *out, const char *filename) {
    assert(out != NULL);
    assert(filename != NULL);

    int width, height, channels;

    stbi_uc *data = stbi_load(filename, &width, &height, &channels, 0);
    if (!data) return false;

    out->width = width;
    out->height = height;
    out->channel_count = channels;
    out->data = data;

    return true;
}

void image_destroy(struct image *img) {
    assert(img != NULL);

    stbi_image_free(img->data);
    memset(img, 0, sizeof(struct image));
}

uint8_t *image_pixel(const struct image *img, int x, int y) {
    assert(img != NULL);
    assert(img->data != NULL);
    assert(x >= 0 && x < img->width);
    assert(y >= 0 && y < img->height);

    return &img->data[(y * img->width + x) * img->channel_count];
}
