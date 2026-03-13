/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Copyright (c) 2026 Pavlo Sytnyk.                                      *
 * Licensed under the MIT License. See LICENSE for license information.  *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "image.h"

#include "util.h"

#include <stb_image.h>

void image_load(const char *filename, struct image *out) {
    out->data = stbi_load(filename, &out->width, &out->height, &out->channel_count, 0);

    if (!out->data) {
        error("File '%s' does not exist", filename);
    }
}
