/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Copyright (c) 2026 Pavlo Sytnyk.                                      *
 * Licensed under the MIT License. See LICENSE for license information.  *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "image_transform.h"

#include "image.h"
#include "util.h"

#include <math.h>
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

static void image_resize_box(const struct image *src, struct image *dst) {
    double sx = (double)src->width / dst->width;
    double sy = (double)src->height / dst->height;

    int channels = src->channel_count;

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

void image_resized(
    struct image *out,
    const struct image *src,
    int width,
    int height,
    enum image_resize_filter filter
) {
    out->width = width;
    out->height = height;
    out->channel_count = src->channel_count;
    out->data = malloc(width * height * out->channel_count);

    switch (filter) {
    case IMAGE_RESIZE_FILTER_NEAREST_NEIGHBOR:
        image_resize_nearest_neighbor(src, out);
        break;
    case IMAGE_RESIZE_FILTER_BOX:
        image_resize_box(src, out);
        break;
    default:
        error("Invalid image resize filter value");
        break;
    }
}
