/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Copyright (c) 2026 Pavlo Sytnyk.                                      *
 * Licensed under the MIT License. See LICENSE for license information.  *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef args_h__
#define args_h__

#include <stdbool.h>

struct args {
    const char *img_filename;
    double contrast;
    int width;
    bool use_weighted_grayscale;
};

void parse_args(int argc, char const *const *argv, struct args *out_args);

#endif // args_h__
