#include <fstream>
#include <sstream>

#include "util.h"

void die(const std::string& msg)
{
    std::cerr << "Fatal error: " << msg << std::endl;
    std::exit(1);
}

void InitShaders()
{
    ProgramShaderId = glCreateProgram();

    CompileAndLoadVertexShader("shader.frag");
    CompileAndLoadVertexShader("shader.vert");

    glAttachShader(ProgramShaderId, VertexShaderId);
    glAttachShader(ProgramShaderId, FragmentShaderId);
    glLinkProgram(ProgramShaderId);

    GLint LinkSuccess;
    char InfoLog[INFOLOG_SIZE];
    glGetProgramiv(ProgramShaderId, GL_LINK_STATUS, &LinkSuccess);
    if (!LinkSuccess) {
        glGetShaderInfoLog(ProgramShaderId, INFOLOG_SIZE, nullptr, InfoLog);
        std::cerr << InfoLog << std::endl;
        die("Linking shaders failed, see above for linker output");
    }

    glDeleteShader(VertexShaderId);
    glDeleteShader(FragmentShaderId);
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

void LoadTextureImage(const std::string& Path, int& Width, int& Height)
{
    int Comp; // number of channels in the file

    unsigned char *Data = stbi_load(
            Path.c_str(),
            &Width,
            &Height,
            &Comp,
            /* desired_channels = */ 0
    );

    glGenTextures(1, &JpegTextureId);
    glBindTexture(GL_TEXTURE_2D, JpegTextureId);
    glActiveTexture(GL_TEXTURE0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, Width, Height, 0, GL_RGB, GL_UNSIGNED_BYTE, Data);

    // TODO Generate mipmap here?
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
