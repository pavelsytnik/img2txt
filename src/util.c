/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Copyright (c) 2026 Pavlo Sytnyk.                                      *
 * Licensed under the MIT License. See LICENSE for license information.  *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "util.h"

#ifdef _WIN32
  #include "win32.h"
#endif

#include <stdio.h>

FILE *img2txt_fopen(const char *filename, const char *mode) {
#ifdef _WIN32
    return win32_fopen(filename, mode);
#else
    return fopen(filename, mode);
#endif
}
