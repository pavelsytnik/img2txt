/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Copyright (c) 2026 Pavlo Sytnyk.                                      *
 * Licensed under the MIT License. See LICENSE for license information.  *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "image_transform.h"

#include "image.h"

#include <assert.h>
#include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

typedef void (*resize_fn)(const struct image *src, struct image *dst);

static void image_resize_nearest_neighbor(const struct image *src, struct image *dst) {
    for (int y = 0; y < dst->height; y++) {
        int y0 = y * src->height / dst->height;

        for (int x = 0; x < dst->width; x++) {
            int x0 = x * src->width / dst->width;

            const uint8_t *src_pixel = image_pixel(src, x0, y0);
            uint8_t *dst_pixel = image_pixel(dst, x, y);

            memcpy(dst_pixel, src_pixel, dst->channels);
        }
    } 
}

static void image_resize_box(const struct image *src, struct image *dst) {
    double sx = (double)src->width / dst->width;
    double sy = (double)src->height / dst->height;

    int channels = src->channels;

    for (int y = 0; y < dst->height; y++) {
        double y0 = y * sy;
        double y1 = (y + 1) * sy;

        int iy0 = (int)floor(y0);
        int iy1 = (int)ceil(y1);

        if (iy1 > src->height) {
            iy1 = src->height;
        }

        for (int x = 0; x < dst->width; x++) {
            double x0 = x * sx;
            double x1 = (x + 1) * sx;

            int ix0 = (int)floor(x0);
            int ix1 = (int)ceil(x1);

            if (ix1 > src->width) {
                ix1 = src->width;
            }

            double acc[4] = {0.0, 0.0, 0.0, 0.0};
            double area = 0.0;

            for (int iy = iy0; iy < iy1; iy++) {
                double wy = fmin(y1, iy + 1.0) - fmax(y0, (double)iy);

                for (int ix = ix0; ix < ix1; ix++) {
                    double wx = fmin(x1, ix + 1.0) - fmax(x0, (double)ix);

                    double w = wx * wy;

                    const uint8_t *src_pixel = image_pixel(src, ix, iy);
                    for (int c = 0; c < channels; c++) {
                        acc[c] += src_pixel[c] * w;
                    }

                    area += w;
                }
            }

            uint8_t *dst_pixel = image_pixel(dst, x, y);

            for (int c = 0; c < channels; c++) {
                int v = (int)round(acc[c] / area);

                if (v < 0) {
                    v = 0;
                } else if (v > 255) {
                    v = 255;
                }

                dst_pixel[c] = (uint8_t)v;
            }
        }
    }
}

static resize_fn get_resize_func(enum image_resize_filter filter) {
    switch (filter) {
    case IMAGE_RESIZE_FILTER_NEAREST_NEIGHBOR:
        return image_resize_nearest_neighbor;
    case IMAGE_RESIZE_FILTER_BOX:
        return image_resize_box;
    default:
        return NULL;
    }
}

bool image_resized(
    struct image *img,
    const struct image *src,
    int width,
    int height,
    enum image_resize_filter filter
) {
    assert(img != NULL);
    assert(src != NULL);
    assert(src->width > 0);
    assert(src->height > 0);
    assert(src->channels >= 1 && src->channels <= 4);
    assert(src->data != NULL);
    assert(width > 0);
    assert(height > 0);

    resize_fn resize = get_resize_func(filter);
    if (!resize) return false;

    if (!image_init(img, width, height, src->channels)) {
        return false;
    }

    (*resize)(src, img);

    return true;
}
