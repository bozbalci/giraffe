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

vec2 ComputeNeighborCoordinates(in vec2 Offset)
{
    float dx = 1.0f / TextureWidth;
    float dz = 1.0f / TextureHeight;

    return vec2(1.0 - (Position.x + Offset.x) * dx, 1.0f - (Position.z + Offset.y) * dz);
}

vec3 ComputeNormal()
{
    // TODO: Refactor
    vec3 Left = vec3(Position.x - 1.0f , texture(HeightMap, ComputeNeighborCoordinates(vec2(-1.0f, 0.0f))).r, Position.z);
    vec3 Right = vec3(Position.x + 1.0f, texture(HeightMap, ComputeNeighborCoordinates(vec2(1.0f, 0.0f))).r, Position.z);
    vec3 Top = vec3(Position.x , texture(HeightMap, ComputeNeighborCoordinates(vec2(0.0f, -1.0f))).r, Position.z - 1.0f);
    vec3 Bottom = vec3(Position.x , texture(HeightMap, ComputeNeighborCoordinates(vec2(0.0f, 1.0f))).r, Position.z + 1.0f);
    vec3 TopRight = vec3(Position.x + 1.0f, texture(HeightMap, ComputeNeighborCoordinates(vec2(1.0f, 1.0f))).r, Position.z - 1.0f);
    vec3 BottomLeft = vec3(Position.x - 1.0f, texture(HeightMap, ComputeNeighborCoordinates(vec2(-1.0f, -1.0f))).r, Position.z + 1.0f);

    vec3 Pos = vec3(Position.x, texture(HeightMap, TextureCoordinate).r, Position.z);
    vec3 Norm = vec3(0.0f, 0.0f, 0.0f);
    Norm += cross(Right - Pos, TopRight - Pos);
    Norm += cross(TopRight - Pos, Top - Pos);
    Norm += cross(Top - Pos, Left - Pos);
    Norm += cross(Left - Pos, BottomLeft - Pos);
    Norm += cross(BottomLeft - Pos, Bottom - Pos);
    Norm += cross(Bottom - Pos, Right - Pos);
    return normalize(Norm);
}

void main()
{
    vec4 TextureColor = texture(Texture, TextureCoordinate);

    float Height = texture(HeightMap, TextureCoordinate).r;

    VertexNormal = ComputeNormal();

    ToLight = normalize(LightPosition - vec3(Position.x, Height, Position.z));
    ToCamera = normalize(CameraPosition.xyz - vec3(Position.x, Height, Position.z));

    VertexTextureCoordinate = TextureCoordinate;
    gl_Position = MVPMatrix * vec4(Position.x, Height, Position.z, 1.0f);
}
