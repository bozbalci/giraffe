#ifndef HW3_UTIL_H
#define HW3_UTIL_H

#include <string>
#include <iostream>

#include <GLFW/glfw3.h>

#include "stb_image.h"
#include "globals.h"

constexpr auto INFOLOG_SIZE = 512;

void die(const std::string& msg);

void InitShaders();

void CompileAndLoadVertexShader(const std::string& Path);

void CompileAndLoadFragmentShader(const std::string& Path);

void LoadTextureImage(const std::string& Path, int& Width, int& Height);

bool ReadFileIntoBuffer(const std::string& Path, std::stringstream& Buffer);

#endif //HW3_UTIL_H
