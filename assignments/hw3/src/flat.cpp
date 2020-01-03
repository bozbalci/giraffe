#include <GL/glew.h>

#include "glm/glm.hpp"
#include "globals.h"

#define STB_IMAGE_IMPLEMENTATION
#define STBI_ASSERT(x)
#include "stb_image.h"

#include <iostream>
#include <GLFW/glfw3.h>
#include <vector>

static void errorCallback(int error, const char * description) {
    fprintf(stderr, "Error: %s\n", description);
}

struct vertex {
    float xPos;
    float yPos;
    float zPos;
    float xTexPos;
    float yTexPos;
};

int TextureWidth;
int TextureHeight;

void initVerticesV(std::vector<vertex> & vertices) {
    for (int i = 0; i <= TextureHeight; i++) {
        for (int  j = 0; j <= TextureWidth; j++) {
            vertices[i * TextureWidth + j].xPos = (float) j;
            vertices[i * TextureWidth + j].yPos = 0.0f;
            vertices[i * TextureWidth + j].zPos = (float) i;
            vertices[i * TextureWidth + j].xTexPos = (float) -j / TextureWidth;
            vertices[i * TextureWidth + j].yTexPos = (float) -i / TextureHeight;
        }
    }
}

int main()
{
    int x, y, n;
    unsigned char *data = stbi_load("../materials/normal_earth_med.jpg", &x, &y, &n, 0);
    if (!data) {
        return 1;
    }

    glfwSetErrorCallback(errorCallback);
        
    if (!glfwInit()) {
        exit(-1);
    }
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

        
    auto win = glfwCreateWindow(800, 800, "CENG477 - HW3", nullptr, nullptr);
    
    if (!win) {
        glfwTerminate();
        exit(-1);
    }
    
    glfwMakeContextCurrent(win);
    
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
        glfwTerminate();
        exit(-1);
    }
}
