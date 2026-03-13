/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Copyright (c) 2026 Pavlo Sytnyk.                                      *
 * Licensed under the MIT License. See LICENSE for license information.  *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef image_h__
#define image_h__

#include <stb_image.h>

struct image {
    int width;
    int height;
    int channel_count;
    stbi_uc *data;
};

void image_load(const char *filename, struct image *out);

#endif // image_h__
