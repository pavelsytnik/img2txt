/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Copyright (c) 2026 Pavlo Sytnyk.                                      *
 * Licensed under the MIT License. See LICENSE for license information.  *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef image_transform__h_
#define image_transform__h_

#include <stdbool.h>

enum image_resize_filter {
    IMAGE_RESIZE_FILTER_NEAREST_NEIGHBOR,
    IMAGE_RESIZE_FILTER_BOX
};

struct image;

bool image_resized(
    struct image *img,
    const struct image *src,
    int width,
    int height,
    enum image_resize_filter filter
);

#endif // image_transform__h_
