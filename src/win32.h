/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Copyright (c) 2026 Pavlo Sytnyk.                                      *
 * Licensed under the MIT License. See LICENSE for license information.  *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifdef _WIN32

#ifndef win32__h_
#define win32__h_

#include <stdio.h>

FILE *win32_fopen(const char *filename, const char *mode);

void win32_console_enable_utf8(void);

char **win32_argv_fetch(void);
void win32_argv_free(char **argv);

#endif // win32__h_

#endif // _WIN32
