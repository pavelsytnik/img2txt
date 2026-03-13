/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Copyright (c) 2026 Pavlo Sytnyk.                                      *
 * Licensed under the MIT License. See LICENSE for license information.  *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef ascii_art_h__
#define ascii_art_h__

#include "image.h"

#include <stdbool.h>
#include <stdio.h>

void write_ascii_art(
    FILE *dst,
    const struct image *img,
    int out_width,
    double contrast,
    bool use_weighted_grayscale
);

#endif // ascii_art_h__
