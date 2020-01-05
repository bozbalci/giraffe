#ifndef HW3_UTIL_H
#define HW3_UTIL_H

#include <string>
#include <iostream>


constexpr auto INFOLOG_SIZE = 512;

void die(const std::string& msg);

void InitializeShaders();

void CompileAndLoadVertexShader(const std::string& Path);

void CompileAndLoadFragmentShader(const std::string& Path);

void LoadTextures(const std::string& HeightMapPath, const std::string& TexturePath, int& Width, int& Height);

bool ReadFileIntoBuffer(const std::string& Path, std::stringstream& Buffer);


#endif //HW3_UTIL_H
