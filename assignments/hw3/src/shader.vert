#version 410
layout(location = 0) in vec3 Position;
layout(location = 1) in vec2 TextureCoordinate;

uniform mat4 MVPMatrix;

// Texture-related data
uniform sampler2D Texture;
uniform sampler2D HeightMap;
uniform int TextureWidth;
uniform int TextureHeight;
uniform vec3 CameraPosition;
uniform vec3 LightPosition;


// Output to Fragment Shader
out vec2 VertexTextureCoordinate; // For texture-color
out vec3 VertexNormal; // For Lighting computation
out vec3 ToLight; // Vector from Vertex to Light;
out vec3 ToCamera; // Vector from Vertex to Camera;


void main()
{
    vec4 TextureColor = texture(Texture, TextureCoordinate);

    float Height = texture(HeightMap, TextureCoordinate).x;

    VertexNormal = vec3(1, 1, 1);

    ToLight = normalize(LightPosition - vec3(Position.x, Height, Position.z));

    ToCamera = normalize(CameraPosition.xyz - vec3(Position.x, Height, Position.z));

    gl_Position = MVPMatrix * vec4(Position.x, Height, Position.z, 1.0f);
}
