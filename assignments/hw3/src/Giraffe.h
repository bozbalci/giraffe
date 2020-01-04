#ifndef HW3_GIRAFFE_H
#define HW3_GIRAFFE_H

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

extern GLuint ProgramShaderId;
extern GLuint FragmentShaderId;
extern GLuint VertexShaderId;
extern GLuint DiffuseTextureId;
extern GLuint HeightMapTextureId;

#endif //HW3_GIRAFFE_H
