/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Copyright (c) 2026 Pavlo Sytnyk.                                      *
 * Licensed under the MIT License. See LICENSE for license information.  *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef util_h__
#define util_h__

#include <stdio.h>
#include <stdnoreturn.h>

noreturn void usage(void);
noreturn void error(const char *fmt, ...);

FILE *img2txt_fopen(const char *filename, const char *mode);

#endif // util_h__
