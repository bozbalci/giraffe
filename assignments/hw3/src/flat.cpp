#include "glm/glm.hpp"
#include "globals.h"

#define STB_IMAGE_IMPLEMENTATION
#define STBI_ASSERT(x)
#include "stb_image.h"

#include <iostream>

int main()
{
    int x, y, n;
    unsigned char *data = stbi_load("../materials/normal_earth_med.jpg", &x, &y, &n, 0);
    if (!data) {
        return 1;
    }

    for (int i = 0; i < x * y; ++i) {
        std::cout << (unsigned int)data[i] << ' ';
    }

    stbi_image_free(data);
}
