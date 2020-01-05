#include <iostream>
#include <vector>

#include "Giraffe.h"

#include "state.h"
#include "util.h"

GLuint ProgramShaderId;
GLuint FragmentShaderId;
GLuint VertexShaderId;
GLuint DiffuseTextureId;
GLuint HeightMapTextureId;

int TextureWidth;
int TextureHeight;

struct Vertex {
    glm::vec3 Position;
    glm::vec2 TextureCoordinates;
};

static void ErrorCallback(int Error, const char *Description) {
    fprintf(stderr, "Error: %s\n", Description);
}

static void FramebufferSizeCallback(GLFWwindow *Window, int Width, int Height)
{
    glViewport(0, 0, Width, Height);
}

void KeyCallback(GLFWwindow *Window, int Key, int ScanCode, int Action, int Mods)
{
#define ON_KEY(KeyName) if (Key == GLFW_KEY_##KeyName && (Action == GLFW_PRESS || Action == GLFW_REPEAT))

    ON_KEY(R) TheState.IncrementHeightFactor();
    ON_KEY(F) TheState.DecrementHeightFactor();
    ON_KEY(Q) TheState.ShiftTextureLeft();
    ON_KEY(E) TheState.ShiftTextureRight();
    ON_KEY(LEFT) TheState.LightSourceLeft();
    ON_KEY(RIGHT) TheState.LightSourceRight();
    ON_KEY(UP) TheState.LightSourceUp();
    ON_KEY(DOWN) TheState.LightSourceDown();
    ON_KEY(T) TheState.LightSourceIncrementHeight();
    ON_KEY(G) TheState.LightSourceDecrementHeight();
    ON_KEY(W) TheState.IncrementPitch();
    ON_KEY(S) TheState.DecrementPitch();
    ON_KEY(A) TheState.DecrementYaw();
    ON_KEY(D) TheState.IncrementYaw();
    ON_KEY(Y) TheState.IncrementSpeed();
    ON_KEY(H) TheState.DecrementSpeed();
    ON_KEY(X) TheState.ResetSpeed();
    ON_KEY(I) TheState.ResetPositionAndCamera();

    // TODO P for Fullscreen

    if (glfwGetKey(Window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(Window, true);
    }

#undef ON_KEY
}

std::vector<Vertex> GenerateTerrainVertices()
{
    std::vector<Vertex> TerrainVertices;

    for (auto z = 0; z <= TextureHeight; ++z) {
        for (auto x = 0; x <= TextureWidth; ++x) {
            TerrainVertices.push_back({
                .Position = glm::vec3((float) x, 0.0f, (float) z),
                .TextureCoordinates = glm::vec2((float) -x / TextureWidth,
                                                (float) -z / TextureHeight)
            });
        }
    }

    return TerrainVertices;
}

std::vector<int> GenerateTerrainIndices()
{
    std::vector<int> TerrainIndices;

    for (auto i = 0; i < TextureHeight - 1; ++i) {
        for (auto j = 0; j < TextureWidth - 1; ++j) {
            auto Current = i * TextureWidth + j;
            auto Right = Current + 1;
            auto Bottom = Current + TextureWidth;
            auto BottomRight = Bottom + 1;

            TerrainIndices.push_back(Current);
            TerrainIndices.push_back(Right);
            TerrainIndices.push_back(Bottom);
            TerrainIndices.push_back(Right);
            TerrainIndices.push_back(BottomRight);
            TerrainIndices.push_back(Bottom);
        }
    }

    return TerrainIndices;
}

int main(int argc, char **argv)
{
    if (argc < 3) {
        die("usage: ./hw3_flat [path_to_height_map] [path_to_texture_map]");
    }

    auto HeightMapPath = argv[1];
    auto TexturePath = argv[2];

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

    auto Window = glfwCreateWindow(WINDOW_SIZE, WINDOW_SIZE, "CENG477", nullptr, nullptr);

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

    InitializeShaders();
    LoadTextures(HeightMapPath, TexturePath, TextureWidth, TextureHeight);
    auto Vertices = GenerateTerrainVertices();
    auto Indices = GenerateTerrainIndices();

    GLuint VAO;
    glGenVertexArrays(/* n = */ 1, &VAO);
    glBindVertexArray(VAO);

    GLuint VBO;
    glGenBuffers(/* n = */ 1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    GLuint EBO;
    glGenBuffers(/* n = */ 1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

    glBufferData(
        GL_ARRAY_BUFFER,
        Vertices.size() * sizeof(Vertex),
        Vertices.data(),
        GL_STATIC_DRAW
    );

    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        Indices.size() * sizeof(int),
        Indices.data(),
        GL_STATIC_DRAW
    );

    glVertexAttribPointer(
        /* index = */ 0,
        /* size = */ 3,
        GL_FLOAT,
        GL_FALSE,
        sizeof(Vertex),
        reinterpret_cast<void *>(0)
    );
    glVertexAttribPointer(
        /* index = */ 1,
        /* size = */ 2,
        GL_FLOAT,
        GL_FALSE,
        sizeof(Vertex),
        reinterpret_cast<void *>(offsetof(Vertex, TextureCoordinates))
    );

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    // Set callbacks
    glfwSetKeyCallback(Window, KeyCallback);
    glfwSetErrorCallback(ErrorCallback);
    glfwSetFramebufferSizeCallback(Window, FramebufferSizeCallback);

    glEnable(GL_DEPTH_TEST);

    TheState.Initialize(TextureWidth, TextureHeight);

    while (!glfwWindowShouldClose(Window)) {
        auto BGColor = glm::min(1.0f, glm::abs(TheState.Speed) / 5.0f);
        glClearColor(BGColor, BGColor, BGColor, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        TheState.Update();
#ifdef DEBUG
        TheState.Print();
#endif

        glDrawElements(GL_TRIANGLES, Indices.size(), GL_UNSIGNED_INT, 0);
        glfwSwapBuffers(Window);
        glfwPollEvents();
    }

    glfwDestroyWindow(Window);
    glfwTerminate();

    return 0;
}
