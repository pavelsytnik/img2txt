/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Copyright (c) 2026 Pavlo Sytnyk.                                      *
 * Licensed under the MIT License. See LICENSE for license information.  *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef text_art__h_
#define text_art__h_

#include "image.h"

#include <stdbool.h>
#include <stdio.h>

#define TEXT_ART_ASCII_RAMP_STANDARD "@%#*+=-:. "
#define TEXT_ART_BLOCK_RAMP "█▓▒░ "

struct text_art {
    char *buffer;
    int width;
    int height;
};

struct text_art_config {
    const char *ramp;
    int out_width;
    double contrast;
    bool use_weighted_grayscale;
    bool box_filter;
};

void text_art_create(struct text_art *art,
    const struct image *img,
    const struct text_art_config *config
);
void text_art_destroy(struct text_art *art);
void text_art_write(const struct text_art *art, FILE *stream);

#endif // text_art__h_
