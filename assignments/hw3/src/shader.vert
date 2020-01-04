#version 330 core

layout(location = 0) in vec3 Position;
layout(location = 1) in vec2 TextureCoordinate;

uniform mat4 MVPMatrix;

uniform sampler2D Texture;
uniform sampler2D HeightMap;
uniform int TextureWidth;
uniform int TextureHeight;
uniform vec3 CameraPosition;
uniform vec3 LightPosition;

out vec2 VertexTextureCoordinate;
out vec3 VertexNormal;
out vec3 ToLight;
out vec3 ToCamera;

void main()
{
    vec4 TextureColor = texture(Texture, TextureCoordinate);

    float Height = texture(HeightMap, TextureCoordinate).x;

    VertexNormal = vec3(1, 1, 1); // TODO

    ToLight = normalize(LightPosition - vec3(Position.x, Height, Position.z));
    ToCamera = normalize(CameraPosition.xyz - vec3(Position.x, Height, Position.z));

    VertexTextureCoordinate = TextureCoordinate;
    gl_Position = MVPMatrix * vec4(Position.x, Height, Position.z, 1.0f);
}
