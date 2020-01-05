#include <fstream>
#include <sstream>

#include "Giraffe.h"

#include "util.h"

#define STB_IMAGE_IMPLEMENTATION
#define STBI_ASSERT(x)
#include "stb_image.h"


void die(const std::string& msg)
{
    std::cerr << "Fatal error: " << msg << std::endl;
    std::exit(1);
}

void InitializeShaders()
{
    ProgramShaderId = glCreateProgram();

    CompileAndLoadVertexShader("shader.vert");
    CompileAndLoadFragmentShader("shader.frag");

    glAttachShader(ProgramShaderId, VertexShaderId);
    glAttachShader(ProgramShaderId, FragmentShaderId);
    glLinkProgram(ProgramShaderId);

    GLint LinkSuccess;
    char InfoLog[INFOLOG_SIZE];
    glGetProgramiv(ProgramShaderId, GL_LINK_STATUS, &LinkSuccess);
    if (!LinkSuccess) {
        glGetProgramInfoLog(ProgramShaderId, INFOLOG_SIZE, nullptr, InfoLog);
        std::cerr << InfoLog << std::endl;
        die("Linking shaders failed, see above for linker output");
    }

    glDeleteShader(VertexShaderId);
    glDeleteShader(FragmentShaderId);

    glUseProgram(ProgramShaderId);
}

void CompileAndLoadVertexShader(const std::string& Path)
{
    std::stringstream ShaderSourceBuffer;
    std::string ShaderSource;

    if (!ReadFileIntoBuffer(Path, ShaderSourceBuffer))
        die("Could not read vertex shader source");

    ShaderSource = ShaderSourceBuffer.str();
    GLint SourceLength = ShaderSource.length();
    auto SourceBytes = (const GLchar *) ShaderSource.c_str();
    VertexShaderId = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(VertexShaderId, 1, &SourceBytes, &SourceLength);
    glCompileShader(VertexShaderId);

    GLint CompilationSuccess;
    char InfoLog[INFOLOG_SIZE];
    glGetShaderiv(VertexShaderId, GL_COMPILE_STATUS, &CompilationSuccess);
    if (!CompilationSuccess) {
        glGetShaderInfoLog(VertexShaderId, INFOLOG_SIZE, nullptr, InfoLog);
        std::cerr << InfoLog << std::endl;
        die("Vertex shader compilation failed, see above for compiler output");
    }
}

void CompileAndLoadFragmentShader(const std::string& Path)
{
    std::stringstream ShaderSourceBuffer;
    std::string ShaderSource;

    if (!ReadFileIntoBuffer(Path, ShaderSourceBuffer))
        die("Could not read vertex shader source");

    ShaderSource = ShaderSourceBuffer.str();
    GLint SourceLength = ShaderSource.length();
    auto SourceBytes = (const GLchar *) ShaderSource.c_str();
    FragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(FragmentShaderId, 1, &SourceBytes, &SourceLength);
    glCompileShader(FragmentShaderId);

    GLint CompilationSuccess;
    char InfoLog[INFOLOG_SIZE];
    glGetShaderiv(FragmentShaderId, GL_COMPILE_STATUS, &CompilationSuccess);
    if (!CompilationSuccess) {
        glGetShaderInfoLog(FragmentShaderId, INFOLOG_SIZE, nullptr, InfoLog);
        std::cerr << InfoLog << std::endl;
        die("Fragment shader compilation failed, see above for compiler output");
    }
}

void LoadTextures(
    const std::string& HeightMapPath,
    const std::string& TexturePath,
    int& Width,
    int& Height
) {
    int Comp; // number of channels in the file being read
    GLuint Textures[2];
    glGenTextures(/* n = */ 2, Textures);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, Textures[0]);

    unsigned char *Data = stbi_load(
        HeightMapPath.c_str(),
        &Width,
        &Height,
        &Comp,
        /* desired_channels = */ 0
    );

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, Width, Height, 0, GL_RGB, GL_UNSIGNED_BYTE, Data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, Textures[1]);

    Data = stbi_load(
        TexturePath.c_str(),
        &Width,
        &Height,
        &Comp,
        /* desired_channels = */ 0
    );
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, Width, Height, 0, GL_RGB, GL_UNSIGNED_BYTE, Data);
    glGenerateMipmap(GL_TEXTURE_2D);

    auto HeightMapLocation = glGetUniformLocation(ProgramShaderId, "HeightMap");
    auto TextureLocation = glGetUniformLocation(ProgramShaderId, "Texture");

    glUniform1i(HeightMapLocation, /* v0 = */ 0);
    glUniform1i(TextureLocation, /* v0 = */ 1);
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
