/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Copyright (c) 2026 Pavlo Sytnyk.                                      *
 * Licensed under the MIT License. See LICENSE for license information.  *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef image_h__
#define image_h__

#include <stb_image.h>

#include <stdint.h>

struct image {
    int width;
    int height;
    int channel_count;
    stbi_uc *data;
};

void image_load(const char *filename, struct image *out);
void image_free(struct image *img);

const uint8_t *image_pixel(const struct image *img, int x, int y);

#endif // image_h__
