/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Copyright (c) 2026 Pavlo Sytnyk.                                      *
 * Licensed under the MIT License. See LICENSE for license information.  *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "args.h"
#include "ascii_art.h"
#include "image.h"
#include "util.h"

#include <stb_image.h>

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
        stbi_image_free(img.data);
        error("Failed to create file '%s'", txt_art_filename);
    }

    write_ascii_art(txt_art_file, &img, args.width, args.contrast, args.use_weighted_grayscale);

    fclose(txt_art_file);
    stbi_image_free(img.data);

    return EXIT_SUCCESS;
}
