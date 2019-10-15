#ifndef _IMAGE_H_
#define _IMAGE_H_

#include <cstdio>
#include <cstdlib>

#include "defs.h"

typedef union Color {
    struct {
        unsigned char red;
        unsigned char grn;
        unsigned char blu;
    };

    unsigned char channel[3];
} Color;

class Image
{
  public:
    Color **data;
    int width;
    int height;

    Image(int width, int height);
    void setPixelValue(int col, int row, const Color &color);
    void saveImage(const char *imageName) const;
};

#endif
