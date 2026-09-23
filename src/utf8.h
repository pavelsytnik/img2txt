/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Copyright (c) 2026 Pavlo Sytnyk.                                      *
 * Licensed under the MIT License. See LICENSE for license information.  *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef utf8__h_
#define utf8__h_

#include <stddef.h>

struct utf8_char_view {
    const char *data;
    size_t size;
};

size_t utf8_length(const char *s);
size_t utf8_char_length(const char *s);

struct utf8_char_view *utf8_char_views(const char *s);

#endif // utf8__h_
