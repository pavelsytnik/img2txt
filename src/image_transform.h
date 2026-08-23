/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Copyright (c) 2026 Pavlo Sytnyk.                                      *
 * Licensed under the MIT License. See LICENSE for license information.  *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef image_transform__h_
#define image_transform__h_

enum image_resize_filter {
    IMAGE_RESIZE_FILTER_NEAREST_NEIGHBOR
};

struct image;

void image_resize(
    const struct image *src,
    struct image *dst,
    int width,
    int height,
    enum image_resize_filter filter
);

#endif // image_transform__h_
