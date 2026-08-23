/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Copyright (c) 2026 Pavlo Sytnyk.                                      *
 * Licensed under the MIT License. See LICENSE for license information.  *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "image_transform.h"

#include "image.h"
#include "util.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

static void image_resize_nearest_neighbor(const struct image *src, struct image *dst) {
    for (int y = 0; y < dst->height; y++) {
        int y0 = y * src->height / dst->height;

        for (int x = 0; x < dst->width; x++) {
            int x0 = x * src->width / dst->width;

            const uint8_t *src_pixel = image_pixel(src, x0, y0);
            uint8_t *dst_pixel = image_pixel(dst, x, y);

            memcpy(dst_pixel, src_pixel, dst->channel_count);
        }
    } 
}

void image_resize(
    const struct image *src,
    struct image *dst,
    int width,
    int height,
    enum image_resize_filter filter
) {
    dst->width = width;
    dst->height = height;
    dst->channel_count = src->channel_count;
    dst->data = malloc(width * height * dst->channel_count);

    switch (filter) {
    case IMAGE_RESIZE_FILTER_NEAREST_NEIGHBOR:
        image_resize_nearest_neighbor(src, dst);
        break;
    default:
        error("Invalid image resize filter value");
        break;
    }
}
