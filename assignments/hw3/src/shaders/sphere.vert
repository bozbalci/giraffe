#version 330 core

layout(location = 0) in vec3 Position;
layout(location = 1) in vec2 TextureCoordinate;

uniform float HeightFactor;
uniform float TextureHorizontalShift;
uniform int TextureHeight;
uniform int TextureWidth;
uniform mat4 MVPMatrix;
uniform sampler2D HeightMap;
uniform sampler2D Texture;
uniform vec3 CameraPosition;
uniform vec3 LightPosition;

out vec2 VertexTextureCoordinate;
out vec3 ToCamera;
out vec3 ToLight;
out vec3 VertexNormal;

void main()
{
    VertexTextureCoordinate = vec2(TextureCoordinate.x + (TextureHorizontalShift / 125),
                                   TextureCoordinate.y);

    VertexNormal = normalize(Position);
    vec4 HeightMapColor = texture(HeightMap, VertexTextureCoordinate);
    vec4 CalculatedPosition = vec4(Position + HeightFactor * HeightMapColor.r * VertexNormal, 1.0f);

    ToLight = normalize(LightPosition - CalculatedPosition.xyz);
    ToCamera = normalize(CameraPosition - CalculatedPosition.xyz);

    gl_Position = MVPMatrix * CalculatedPosition;
}
