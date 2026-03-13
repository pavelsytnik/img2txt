/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Copyright (c) 2026 Pavlo Sytnyk.                                      *
 * Licensed under the MIT License. See LICENSE for license information.  *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "args.h"
#include "ascii_art.h"
#include "image.h"
#include "util.h"

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
    struct args args;
    parse_args(argc, argv, &args);

    struct image img;
    image_load(args.img_filename, &img);

    char txt_art_filename[256];
    snprintf(txt_art_filename, sizeof(txt_art_filename), "%s.txt", args.img_filename);

    FILE *txt_art_file = fopen(txt_art_filename, "w");

    if (!txt_art_file) {
        image_free(&img);
        error("Failed to create file '%s'", txt_art_filename);
    }

    ascii_art_write(
        txt_art_file,
        &img,
        &(struct ascii_art_config) {
            .out_width = args.width,
            .contrast = args.contrast,
            .use_weighted_grayscale = args.use_weighted_grayscale
        }
    );

    fclose(txt_art_file);
    image_free(&img);

    return EXIT_SUCCESS;
}
