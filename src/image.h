/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Copyright (c) 2026 Pavlo Sytnyk.                                      *
 * Licensed under the MIT License. See LICENSE for license information.  *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef image__h_
#define image__h_

#include <stdint.h>

struct image {
    int width;
    int height;
    int channel_count;
    uint8_t *data;
};

void image_load(struct image *out, const char *filename);
void image_destroy(struct image *img);

uint8_t *image_pixel(const struct image *img, int x, int y);

#endif // image__h_
