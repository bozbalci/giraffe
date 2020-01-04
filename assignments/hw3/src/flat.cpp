#include <iostream>
#include <vector>

#include "Giraffe.h"

#include "util.h"


GLuint ProgramShaderId;
GLuint FragmentShaderId;
GLuint VertexShaderId;
GLuint JpegTextureId;

static void errorCallback(int error, const char * description) {
    fprintf(stderr, "Error: %s\n", description);
}

static void framebufferSizeCallback(GLFWwindow *Window, int Width, int Height)
{
    glViewport(0, 0, Width, Height);
}

void processInput(GLFWwindow *Window)
{
    if (glfwGetKey(Window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(Window, true);
    }
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

int main(int argc, char **argv)
{
    glfwSetErrorCallback(errorCallback);

        
    if (!glfwInit()) {
        die("Could not initialize OpenGL");
    }
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    // Required on macOS.
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    auto Window = glfwCreateWindow(800, 800, "CENG477 - HW3", nullptr, nullptr);
    
    if (!Window) {
        glfwTerminate();
        die("Could not create an OpenGL window");
    }
    
    glfwMakeContextCurrent(Window);

#ifndef __APPLE__
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
        glfwTerminate();
        exit(-1);
    }
#endif

    glfwSetFramebufferSizeCallback(Window, framebufferSizeCallback);

    InitializeShaders();

    /// ---------------------------------------------------------------------------------------
    float vertices[] = {
            -0.5f, -0.5f, 0.0f,
            0.5f, -0.5f, 0.0f,
            0.0f, 0.5f, 0.0f,
    };

    unsigned int VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3,GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *) nullptr);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    while (!glfwWindowShouldClose(Window)) {
        processInput(Window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(ProgramShaderId);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwSwapBuffers(Window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glfwTerminate();

    /// ---------------------------------------------------------------------------------------

    return 0;
}
