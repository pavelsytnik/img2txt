/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Copyright (c) 2026 Pavlo Sytnyk.                                      *
 * Licensed under the MIT License. See LICENSE for license information.  *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "image.h"

#include "util.h"

#include <stb_image.h>

#include <stdint.h>
#include <string.h>

void image_load(const char *filename, struct image *out) {
    out->data = stbi_load(filename, &out->width, &out->height, &out->channel_count, 0);

    if (!out->data) {
        error("File '%s' does not exist", filename);
    }
}

void image_destroy(struct image *img) {
    stbi_image_free(img->data);
    memset(img, 0, sizeof(struct image));
}

const uint8_t *image_pixel(const struct image *img, int x, int y) {
    return &img->data[(y * img->width + x) * img->channel_count];
}
