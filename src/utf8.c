/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Copyright (c) 2026 Pavlo Sytnyk.                                      *
 * Licensed under the MIT License. See LICENSE for license information.  *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "utf8.h"

#include <stdint.h>
#include <stdlib.h>

size_t utf8_length(const char *s) {
    size_t n = 0;

    while (*s) {
        if ((*s & 0xC0) != 0x80) n++;
        s++;
    }

    return n;
}

size_t utf8_char_length(const char *s) {
    uint8_t c = (uint8_t)*s;

    if ((c & 0x80) == 0x00) return 1;
    if ((c & 0xE0) == 0xC0) return 2;
    if ((c & 0xF0) == 0xE0) return 3;
    if ((c & 0xF8) == 0xF0) return 4;

    return 0;
}

struct utf8_char_view *utf8_char_views(const char *s) {
    size_t n = utf8_length(s);

    struct utf8_char_view *views = malloc(n * sizeof(*views));
    if (!views) return NULL;

    const char *c = s;
    for (size_t i = 0; i < n; i++) {
        views[i].data = c;
        views[i].size = utf8_char_length(c);

        c += views[i].size;
    }

    return views;
}
