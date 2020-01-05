#version 330 core

layout(location = 0) in vec3 Position;
layout(location = 1) in vec2 TextureCoordinate;

uniform mat4 MVPMatrix;

void main()
{
    vec4 PosVec4 = vec4(Position, 1.0f);
    gl_Position = MVPMatrix * PosVec4;
}
