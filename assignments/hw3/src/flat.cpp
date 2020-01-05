#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

#ifndef __APPLE__
    #include <GL/glew.h>
#else
    #define GLFW_INCLUDE_GLCOREARB
    #define GLFW_INCLUDE_GLEXT
    #ifdef GLFW_INCLUDE_GLCOREARB
        #include <OpenGL/gl3.h>
        #ifdef GLFW_INCLUDE_GLEXT
            #include <OpenGL/gl3ext.h>
        #endif // GLFW_INCLUDE_GLEXT
    #endif // GLFW_INCLUDE_GLCOREARB
#endif // __APPLE__

#include <GLFW/glfw3.h>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#define STB_IMAGE_IMPLEMENTATION
#define STBI_ASSERT(x)
#include "stb_image.h"

// Uncomment the following line to enable debug messages.
// #define DEBUG

GLuint ProgramShaderId;

constexpr auto WINDOW_SIZE = 1000;
constexpr auto INFO_LOG_SIZE = 512;

void die(const std::string& msg)
{
    std::cerr << "Fatal error: " << msg << std::endl;
    std::exit(1);
}

bool ReadFileIntoBuffer(const std::string& Path, std::stringstream& Buffer)
{
    try {
        std::fstream InFile(Path);
        Buffer << InFile.rdbuf();
        return true;
    } catch (...) {
        return false;
    }
}

struct UIState {
    static constexpr auto HEIGHT_FACTOR_INITIAL = 10.0f;
    static constexpr auto HEIGHT_FACTOR_UNIT = 0.5f;
    static constexpr auto TEXTURE_HORIZONTAL_SHIFT_INITIAL = 0.0f;
    static constexpr auto TEXTURE_HORIZONTAL_SHIFT_UNIT = 1.0f;
    static constexpr auto PITCH_INITIAL = 0.0f;
    static constexpr auto PITCH_UNIT = 0.5f;
    static constexpr auto PITCH_MAX = 89.9f;
    static constexpr auto PITCH_MIN = -PITCH_MAX;
    static constexpr auto YAW_INITIAL = 90.0f;
    static constexpr auto YAW_UNIT = 0.5f;
    static constexpr auto YAW_MAX = 360.0f;
    static constexpr auto YAW_MIN = 0.0f;
    static constexpr auto SPEED_INITIAL = 0.0f;
    static constexpr auto SPEED_UNIT = 0.25f;
    static constexpr auto LIGHT_UNIT = 5.0f;

    // OPENGL-RELATED DATA
    int TextureWidth;
    int TextureHeight;
    GLint CameraPositionLocation;
    GLint LightPositionLocation;
    GLint MVPMatrixLocation;
    GLint HeightFactorLocation;

    /// UI PARAMETERS
    double HeightFactor = HEIGHT_FACTOR_INITIAL;
    double TextureHorizontalShift = TEXTURE_HORIZONTAL_SHIFT_INITIAL;
    float Pitch = PITCH_INITIAL;
    float Yaw = YAW_INITIAL;
    float Speed = SPEED_INITIAL;

    /// UNIFORMS
    struct CameraType {
        glm::vec3 Position;
        glm::vec3 Gaze{0.0f, 0.0f, 0.1f};
        glm::vec3 Up{0.0f, 1.0f, 0.0f};
    } Camera;

    CameraType CAMERA_INITIAL;

    struct {
        glm::mat4 Model{1.0f};
        glm::mat4 View;
        glm::mat4 Projection = glm::perspective(
            glm::radians(45.0f),
            1.0f,
            0.1f,
            1000.0f
        );
        glm::mat4 Composite;
    } Matrices;

    glm::vec3 LightPosition;

    // METHODS
    void IncrementHeightFactor() {
        HeightFactor += HEIGHT_FACTOR_UNIT;
    }

    void DecrementHeightFactor() {
        HeightFactor -= HEIGHT_FACTOR_UNIT;
    }

    void ShiftTextureLeft() {
        TextureHorizontalShift -= TEXTURE_HORIZONTAL_SHIFT_UNIT;
    }

    void ShiftTextureRight() {
        TextureHorizontalShift += TEXTURE_HORIZONTAL_SHIFT_UNIT;
    }

    void LightSourceUp() {
        LightPosition.z += LIGHT_UNIT;
    }

    void LightSourceDown() {
        LightPosition.z -= LIGHT_UNIT;
    }

    void LightSourceLeft() {
        LightPosition.x += LIGHT_UNIT;
    }

    void LightSourceRight() {
        LightPosition.x -= LIGHT_UNIT;
    }

    void LightSourceIncrementHeight() {
        LightPosition.y += LIGHT_UNIT;
    }

    void LightSourceDecrementHeight() {
        LightPosition.y -= LIGHT_UNIT;
    }

    void IncrementPitch() {
        Pitch += PITCH_UNIT;
        if (Pitch > PITCH_MAX) Pitch = PITCH_MAX;
    }

    void DecrementPitch() {
        Pitch -= PITCH_UNIT;
        if (Pitch < PITCH_MIN) Pitch = PITCH_MIN;
    }

    void IncrementYaw() {
        Yaw += YAW_UNIT;
        if (Yaw > YAW_MAX) Yaw -= YAW_MAX;
    }

    void DecrementYaw() {
        Yaw -= YAW_UNIT;
        if (Yaw < YAW_MIN) Yaw += YAW_MAX;
    }

    void IncrementSpeed() {
        Speed += SPEED_UNIT;
    }

    void DecrementSpeed() {
        Speed -= SPEED_UNIT;
    }

    void ResetSpeed() {
        Speed = SPEED_INITIAL;
    }

    void ResetPositionAndCamera() {
        ResetSpeed();
        Pitch = PITCH_INITIAL;
        Yaw = YAW_INITIAL;
        Camera = CAMERA_INITIAL;
    }

    void Initialize(const int& TextureWidth_, const int& TextureHeight_) {
        // Initialize OpenGL variables
        TextureWidth = TextureWidth_;
        TextureHeight = TextureHeight_;
        CameraPositionLocation = glGetUniformLocation(ProgramShaderId, "CameraPosition");
        LightPositionLocation = glGetUniformLocation(ProgramShaderId, "LightPosition");
        MVPMatrixLocation = glGetUniformLocation(ProgramShaderId, "MVPMatrix");
        HeightFactorLocation = glGetUniformLocation(ProgramShaderId, "HeightFactor");

        // Initialize world data
        Camera.Position = {
            (float) TextureWidth / 2.0f,
            (float) TextureWidth / 10.0f,
            (float) -TextureWidth / 4.0f
        };
        LightPosition = {
            (float) TextureWidth / 2.0f,
            100.0f,
            (float) TextureHeight / 2.0f
        };

        // Save this incarnation of the camera so that it can be restored later
        CAMERA_INITIAL = Camera;
    }

    void UpdateCamera() {
        auto YawInRadians = glm::radians(Yaw);
        auto PitchInRadians = glm::radians(Pitch);

        Camera.Gaze = glm::normalize(glm::vec3(
            std::cos(YawInRadians) * std::cos(PitchInRadians),
            std::sin(PitchInRadians),
            std::sin(YawInRadians) * std::cos(PitchInRadians)
        ));

        Camera.Position += Speed * Camera.Gaze;

        glUniform3fv(CameraPositionLocation, 1, glm::value_ptr(Camera.Position));
    }

    void UpdateMVPMatrix() {
        Matrices.View = glm::lookAt(
            Camera.Position,
            Camera.Position + Camera.Gaze,
            Camera.Up
        );

        Matrices.Composite = Matrices.Projection * Matrices.View * Matrices.Model;

        glUniformMatrix4fv(MVPMatrixLocation, 1, GL_FALSE, glm::value_ptr(Matrices.Composite));
    }

    void UpdateHeightFactor() {
        glUniform1f(HeightFactorLocation, HeightFactor);
    }

    void UpdateLightPosition() {
        glUniform3fv(LightPositionLocation, 1, glm::value_ptr(LightPosition));
    }

    void Update() {
        UpdateCamera();
        UpdateMVPMatrix();
        UpdateHeightFactor();
        UpdateLightPosition();
        // TODO Update texture horizontal offset (Q/E)
    }

#ifdef DEBUG
    void Print() {
        system("clear");
        std::cout << "HeightFactor = " << HeightFactor << '\n';
        std::cout << "TextureHorizontalShift = " << TextureHorizontalShift << '\n';
        std::cout << "CameraPosition = {"
                  << Camera.Position.x << ", "
                  << Camera.Position.y << ", "
                  << Camera.Position.z << "}" << '\n';
        std::cout << "LightPosition = {"
                  << LightPosition.x << ", "
                  << LightPosition.y << ", "
                  << LightPosition.z << "}" << '\n';
        std::cout << "Pitch = " << Pitch << '\n';
        std::cout << "Yaw = " << Yaw << '\n';
        std::cout << "Speed = " << Speed << '\n';
    }
#endif
} TheState;

// OPENGL CALLBACKS
void ErrorCallback(int Error, const char *Description) {
    fprintf(stderr, "Error: %s\n", Description);
}

void FramebufferSizeCallback(GLFWwindow *Window, int Width, int Height)
{
    glViewport(/* x = */ 0, /* y = */ 0, Width, Height);
}

void KeyCallback(GLFWwindow *Window, int Key, int ScanCode, int Action, int Mods)
{
#define ON_KEY(KeyName) \
    if (Key == GLFW_KEY_##KeyName \
        && (Action == GLFW_PRESS || Action == GLFW_REPEAT))

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

struct Vertex {
    glm::vec3 Position;
    glm::vec2 TextureCoordinates;
};

class Shader
{
public:
    std::string Path;
    GLenum ShaderType;

    Shader(const std::string &Path_, GLenum ShaderType_)
        : Path(Path_), ShaderType(ShaderType_) {}

    GLuint Load() {
        std::stringstream ShaderSourceBuffer;
        std::string ShaderSource;
        const GLchar *SourceBytes;
        GLint SourceLength;

        if (!ReadFileIntoBuffer(Path, ShaderSourceBuffer))
            die("Could not read shader source");

        ShaderSource = ShaderSourceBuffer.str();
        SourceLength = ShaderSource.length();
        SourceBytes = (const GLchar *) ShaderSource.c_str();
        ShaderId = glCreateShader(ShaderType);
        glShaderSource(ShaderId, /* count = */ 1, &SourceBytes, &SourceLength);
        glCompileShader(ShaderId);

        CheckErrors();

        return ShaderId;
    }
private:
    GLuint ShaderId;

    void CheckErrors() {
        GLint CompilationSuccess;
        char InfoLog[INFO_LOG_SIZE];

        glGetShaderiv(ShaderId, GL_COMPILE_STATUS, &CompilationSuccess);
        if (!CompilationSuccess) {
            glGetShaderInfoLog(ShaderId, INFO_LOG_SIZE, /* length = */ nullptr, InfoLog);
            std::cerr << InfoLog << std::endl;
            die("Could not compile shader, see above for errors");
        }
    }
};

class HW3Utility {
public:
    // INPUT PATHS
    std::string VertexShaderPath = "shader.vert";
    std::string FragmentShaderPath = "shader.frag";
    std::string HeightMapPath;
    std::string TexturePath;

    // TEXTURE PARAMETERS
    int TextureWidth;
    int TextureHeight;
    std::vector<Vertex> Vertices;
    std::vector<int> Indices;

    // OPENGL VARIABLES
    GLFWwindow *Window;
    GLuint VertexArrayObject;
    GLuint VertexBufferObject;
    GLuint ElementBufferObject;

    HW3Utility(const std::string &HeightMapPath_, const std::string &TexturePath_)
        : HeightMapPath(HeightMapPath_), TexturePath(TexturePath_) {}

    // OPENGL ROUTINES
    void InitializeWindow() {
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
        Window = glfwCreateWindow(WINDOW_SIZE, WINDOW_SIZE, "CENG477", nullptr, nullptr);

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
    }

    void InitializeProgram() {
        ProgramShaderId = glCreateProgram();
        GLuint VertexShaderId = Shader(VertexShaderPath, GL_VERTEX_SHADER).Load();
        GLuint FragmentShaderId = Shader(FragmentShaderPath, GL_FRAGMENT_SHADER).Load();

        glAttachShader(ProgramShaderId, VertexShaderId);
        glAttachShader(ProgramShaderId, FragmentShaderId);
        glLinkProgram(ProgramShaderId);

        // Check for linking errors
        GLint LinkSuccess;
        char InfoLog[INFO_LOG_SIZE];
        glGetProgramiv(ProgramShaderId, GL_LINK_STATUS, &LinkSuccess);
        if (!LinkSuccess) {
            glGetProgramInfoLog(ProgramShaderId, INFO_LOG_SIZE, nullptr, InfoLog);
            std::cerr << InfoLog << std::endl;
            die("Linking shaders failed, see above for linker output");
        }

        // Cleanup shaders
        glDeleteShader(VertexShaderId);
        glDeleteShader(FragmentShaderId);

        // Load program
        glUseProgram(ProgramShaderId);

        // Misc. OpenGL options
        glEnable(GL_DEPTH_TEST);
    }

    void InitializeTextures() {
        int Comp; // Number of channels in the file being read
        unsigned char *ImageData;
        GLuint Textures[2];

        glGenTextures(/* n = */ 2, Textures);

        // Load height map
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, Textures[0]);
        ImageData = stbi_load(HeightMapPath.c_str(), &TextureWidth, &TextureHeight, &Comp,
                              /* desired_channels = */ 0);
        glTexImage2D(GL_TEXTURE_2D, /* level = */ 0, GL_RGB, TextureWidth, TextureHeight,
                     /* border = */ 0, GL_RGB, GL_UNSIGNED_BYTE, ImageData);
        glGenerateMipmap(GL_TEXTURE_2D);

        // Load diffuse map
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, Textures[0]);
        ImageData = stbi_load(TexturePath.c_str(), &TextureWidth, &TextureHeight, &Comp,
                              /* desired_channels = */ 0);
        glTexImage2D(GL_TEXTURE_2D, /* level = */ 0, GL_RGB, TextureWidth, TextureHeight,
                     /* border = */ 0, GL_RGB, GL_UNSIGNED_BYTE, ImageData);
        glGenerateMipmap(GL_TEXTURE_2D);

        auto HeightMapLocation = glGetUniformLocation(ProgramShaderId, "HeightMap");
        auto TextureLocation = glGetUniformLocation(ProgramShaderId, "Texture");

        glUniform1i(HeightMapLocation, /* v0 = */ 0);
        glUniform1i(TextureLocation, /* v0 = */ 1);
    }

    void InitializeVertexArrayObject() {
        glGenVertexArrays(/* n = */ 1, &VertexArrayObject);
        glBindVertexArray(VertexArrayObject);
    }

    void InitializeVertexBufferObject() {
        glGenBuffers(/* n = */ 1, &VertexBufferObject);
        glBindBuffer(GL_ARRAY_BUFFER, VertexBufferObject);
    }

    void InitializeElementBufferObject() {
        glGenBuffers(/* n = */ 1, &ElementBufferObject);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ElementBufferObject);
    }

    void LoadVertexDataToBuffers() {
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
    }

    static void InitializeVertexAttribPointer() {
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

        glEnableVertexAttribArray(/* index = */ 0);
        glEnableVertexAttribArray(/* index = */ 1);
    }

    void RegisterCallbacks() {
        glfwSetKeyCallback(Window, KeyCallback);
        glfwSetErrorCallback(ErrorCallback);
        glfwSetFramebufferSizeCallback(Window, FramebufferSizeCallback);
    }

    // MESH GENERATION
    void GenerateTerrainVertices() {
        for (auto z = 0; z < TextureHeight; ++z) {
            for (auto x = 0; x < TextureWidth; ++x) {
                Vertices.push_back({
                   .Position = glm::vec3((float) x, 0.0f, (float) z),
                   .TextureCoordinates = glm::vec2((float) -x / TextureWidth,
                                                   (float) -z / TextureHeight)
               });
            }
        }
    }

    void GenerateTerrainIndices()
    {
        for (auto i = 0; i < TextureHeight - 1; ++i) {
            for (auto j = 0; j < TextureWidth - 1; ++j) {
                auto Current = i * TextureWidth + j;
                auto Right = Current + 1;
                auto Bottom = Current + TextureWidth;
                auto BottomRight = Bottom + 1;

                Indices.push_back(Current);
                Indices.push_back(Right);
                Indices.push_back(Bottom);
                Indices.push_back(Right);
                Indices.push_back(BottomRight);
                Indices.push_back(Bottom);
            }
        }
    }

    // LIFETIME METHODS
    void InitializeState() {
        TheState.Initialize(TextureWidth, TextureHeight);
    }

    void Initialize() {
        InitializeWindow();
        InitializeProgram();
        InitializeTextures();
        GenerateTerrainVertices();
        GenerateTerrainIndices();
        InitializeVertexArrayObject();
        InitializeVertexBufferObject();
        InitializeElementBufferObject();
        LoadVertexDataToBuffers();
        InitializeVertexAttribPointer();
        RegisterCallbacks();
        InitializeState();
    }

    void MainLoop() {
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
    }
};

int main(int argc, char **argv)
{
    if (argc < 3) {
        die("usage: ./hw3_flat [path_to_height_map] [path_to_texture_map]");
    }

    HW3Utility hw3(argv[1], argv[2]);
    hw3.Initialize();
    hw3.MainLoop();

    return 0;
}
