/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Copyright (c) 2026 Pavlo Sytnyk.                                      *
 * Licensed under the MIT License. See LICENSE for license information.  *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef ascii_art_h__
#define ascii_art_h__

#include "image.h"

#include <stdbool.h>
#include <stdio.h>

struct ascii_art_config {
    int out_width;
    double contrast;
    bool use_weighted_grayscale;
};

void ascii_art_write(FILE *dst, const struct image *img, const struct ascii_art_config *config);

#endif // ascii_art_h__
