#include "Image.h"

Image::Image(int width, int height) : width(width), height(height)
{
    data = new Color *[height];

    for (int y = 0; y < height; ++y) {
        data[y] = new Color[width];
    }
}

void Image::setPixelValue(int col, int row, const Color &color)
{
    data[row][col] = color;
}

void Image::saveImage(const char *imageName) const
{
    FILE *output;

    output = fopen(imageName, "w");
    fprintf(output, "P3\n");
    fprintf(output, "%d %d\n", width, height);
    fprintf(output, "255\n");

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            for (int c = 0; c < 3; ++c) {
                fprintf(output, "%d ", data[y][x].channel[c]);
            }
        }

        fprintf(output, "\n");
    }

    fclose(output);
}
