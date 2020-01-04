#version 330 core

out vec4 FragmentColor;

in vec2 VertexTextureCoordinate;
in vec3 VertexNormal;
in vec3 ToLight;
in vec3 ToCamera;

uniform sampler2D HeightMap;
uniform sampler2D Texture;
uniform mat4 MVPMatrix;
uniform int TextureWidth;
uniform int TextureHeight;
uniform vec3 CameraPosition;
uniform vec3 LightPosition;

void main()
{
    // Inputs
    vec4 TextureColor = texture(Texture, VertexTextureCoordinate);

    FragmentColor = vec4(length(TextureColor.xyz), 0, 0, 1);
    return;

    vec4 AmbientReflectance = vec4(0.25, 0.25, 0.25, 1.00);
    vec4 AmbientColor = vec4(0.3, 0.3, 0.3, 1.0);

    vec4 DiffuseReflectance = vec4(1.0, 1.0, 1.0, 1.0);
    vec4 DiffuseColor = vec4(1.0, 1.0, 1.0, 1.0);

    vec4 SpecularReflectance = vec4(1.0, 1.0, 1.0, 1.0);
    vec4 SpecularColor = vec4(1.0, 1.0, 1.0, 1.0);
    float SpecularExponent = 100;

    // Computed values
    vec3 AmbientComponent = (AmbientReflectance * AmbientColor).xyz;

    float DiffuseCoeff = max(dot(VertexNormal, ToLight), 0.0);
    vec3 DiffuseComponent = DiffuseCoeff * (DiffuseReflectance * DiffuseColor).xyz;

    vec3 ReflectedLight = reflect(-ToLight, VertexNormal);
    float SpecularCoeff = pow(max(dot(ReflectedLight, ToCamera), 0.0), SpecularExponent);
    vec3 SpecularComponent = SpecularCoeff * (SpecularReflectance * SpecularColor).xyz;

    vec3 TotalComponent = AmbientComponent + DiffuseComponent + SpecularComponent;
    vec3 UnclampedFragmentColor = TextureColor.xyz * TotalComponent;
    FragmentColor = vec4(clamp(UnclampedFragmentColor, 0.0, 1.0), 1.0);
}
