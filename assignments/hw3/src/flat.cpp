#include <iostream>
#include <vector>

#include "Giraffe.h"

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

struct UIState {
    double HeightFactor = 10.0f;
    double TextureHorizontalShift = 0.0f;
    glm::vec3 LightSourceCoordinate = {1.0f, 1.0f, 1.0f};
    float Pitch = 0.0f;
    float Yaw = 0.0f;
    float Speed = 0.0f;

    void IncrementHeightFactor() {
        HeightFactor += 0.5f;
    }

    void DecrementHeightFactor() {
        HeightFactor -= 0.5f;
    }

    void ShiftTextureLeft() {
        TextureHorizontalShift -= 1.0f;
    }

    void ShiftTextureRight() {
        TextureHorizontalShift += 1.0f;
    }

    void LightSourceUp() {
        LightSourceCoordinate.x += 5.0f;
    }

    void LightSourceDown() {
        LightSourceCoordinate.x -= 5.0f;
    }

    void LightSourceLeft() {
        LightSourceCoordinate.z -= 5.0f;
    }

    void LightSourceRight() {
        LightSourceCoordinate.z += 5.0f;
    }

    void LightSourceIncrementHeight() {
        LightSourceCoordinate.y += 5.0f;
    }

    void LightSourceDecrementHeight() {
        LightSourceCoordinate.y -= 5.0f;
    }

    void IncrementPitch() {
        Pitch += 0.5f;
    }

    void DecrementPitch() {
        Pitch -= 0.5f;
    }

    void IncrementYaw() {
        Yaw += 0.5f;
    }

    void DecrementYaw() {
        Yaw -= 0.5f;
    }

    void IncrementSpeed() {
        Speed += 0.25f;
    }

    void DecrementSpeed() {
        Speed -= 0.25f;
    }

    void ResetSpeed() {
        Speed = 0.0f;
    }

    void Print() {
        system("clear");
        std::cout << "HeightFactor = " << HeightFactor << '\n';
        std::cout << "TextureHorizontalShift = " << TextureHorizontalShift << '\n';
        std::cout << "LightSourceCoordinate = {"
                  << LightSourceCoordinate.x << ", "
                  << LightSourceCoordinate.y << ", "
                  << LightSourceCoordinate.z << "}" << '\n';
        std::cout << "Pitch = " << Pitch << '\n';
        std::cout << "Yaw = " << Yaw << '\n';
        std::cout << "Speed = " << Speed << '\n';
    }
} TheState;


static void errorCallback(int error, const char * description) {
    fprintf(stderr, "Error: %s\n", description);
}

static void framebufferSizeCallback(GLFWwindow *Window, int Width, int Height)
{
    glViewport(0, 0, Width, Height);
}

void ProcessInput(GLFWwindow *Window, int Key, int ScanCode, int Action, int Mods)
{
#define ON_KEY(KeyName) if (Key == GLFW_KEY_##KeyName && (Action == GLFW_PRESS || Action == GLFW_REPEAT))

    ON_KEY(R) TheState.IncrementHeightFactor();
    ON_KEY(F) TheState.DecrementHeightFactor();
    ON_KEY(Q) TheState.ShiftTextureLeft();
    ON_KEY(E) TheState.ShiftTextureRight();
    // TODO Light left-right up-down (arrow keys)
    ON_KEY(T) TheState.LightSourceIncrementHeight();
    ON_KEY(G) TheState.LightSourceDecrementHeight();
    ON_KEY(W) TheState.IncrementPitch();
    ON_KEY(S) TheState.DecrementPitch();
    ON_KEY(A) TheState.IncrementYaw();
    ON_KEY(D) TheState.DecrementYaw();
    ON_KEY(Y) TheState.IncrementSpeed();
    ON_KEY(H) TheState.DecrementSpeed();
    ON_KEY(X) TheState.ResetSpeed();
    // TODO I for Initial Configuration
    // TODO P for Fullscreen

    if (glfwGetKey(Window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(Window, true);
    }
#undef ON_KEY
}

std::vector<Vertex> GenerateTerrainVertices()
{
    std::vector<Vertex> TerrainVertices;

    const auto dx = 1.0f / (float)TextureWidth,
               dz = 1.0f / (float)TextureHeight;

    for (auto z = 0; z <= TextureHeight; ++z) {
        for (auto x = 0; x <= TextureWidth; ++x) {
            TerrainVertices.push_back({
                .Position = glm::vec3(x, 0.0f, z),
                .TextureCoordinates = glm::vec2(1.0f - x * dx, 1.0f - z * dz)
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


    InitializeShaders();

    HeightMapTextureId = LoadTextureImage(HeightMapPath, TextureWidth, TextureHeight, GL_TEXTURE0);
    DiffuseTextureId = LoadTextureImage(TexturePath, TextureWidth, TextureHeight, GL_TEXTURE0 + 1);

    auto HeightMapLocation = glGetUniformLocation(ProgramShaderId, "HeightMap");
    auto TextureLocation = glGetUniformLocation(ProgramShaderId, "Texture");

    glUniform1i(HeightMapLocation, /* v0 = */ 0);
    glUniform1i(TextureLocation, /* v0 = */ 1);

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

    auto CameraPosition = glm::vec3(
        TextureWidth / 2.0f,
        TextureWidth / 10.0f,
        -TextureWidth / 4.0f
    );
    auto CameraPositionLocation = glGetUniformLocation(ProgramShaderId, "CameraPosition");
    glUniform3fv(
        CameraPositionLocation,
        /* count = */ 1,
        glm::value_ptr(CameraPosition)
    );

    auto LightPosition = glm::vec3(
        TextureWidth / 2,
        100,
        TextureHeight / 2
    );
    auto LightPositionLocation = glGetUniformLocation(ProgramShaderId, "LightPosition");
    glUniform3fv(
        LightPositionLocation,
        /* count = */ 1,
        glm::value_ptr(LightPosition)
    );

    auto CameraGaze = glm::vec3(0.0f, 0.0f, 1.0f);
    auto CameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

    auto ModelMatrix = glm::mat4(1.0f);
    auto ViewMatrix = glm::lookAt(
        CameraPosition,
        CameraPosition + CameraGaze,
        CameraUp
    );
    auto ProjectionMatrix = glm::perspective(
        glm::radians(45.0f),
        1.0f,
        0.1f,
        1000.0f
    );
    auto MVPMatrix = ProjectionMatrix * ViewMatrix * ModelMatrix;

    auto MVPMatrixLocation = glGetUniformLocation(ProgramShaderId, "MVPMatrix");
    glUniformMatrix4fv(
        MVPMatrixLocation,
        /* count = */ 1,
        GL_FALSE,
        glm::value_ptr(MVPMatrix)
    );

    // TODO
    glEnable(GL_DEPTH_TEST);

    glfwSetKeyCallback(Window, ProcessInput);
    glfwSetFramebufferSizeCallback(Window, framebufferSizeCallback);

    while (!glfwWindowShouldClose(Window)) {
        glClearColor(1.0f, 1.0f, 0.7f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glDrawElements(
            GL_TRIANGLES,
            Indices.size(),
            GL_UNSIGNED_INT,
            /* indices = */ 0
        );

        glfwSwapBuffers(Window);
        glfwPollEvents();

        TheState.Print();
    }

    glfwDestroyWindow(Window);
    glfwTerminate();

    return 0;
}
