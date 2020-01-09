{% extends "__base.cpp" %}

{% block Variant %}flat{% endblock %}
{% block VertexShaderPath %}shaders/flat.vert{% endblock %}
{% block FragmentShaderPath %}shaders/flat.frag{% endblock %}
{% block HeightFactorInitial %}10.f{% endblock %}

{% block CameraPosition %}
    {
        (float)TextureWidth / 2.0f,
        (float)TextureWidth / 10.0f,
        (float)-TextureWidth / 4.0f
    }
{% endblock %}

{% block CameraGaze %}
    {0.0f, 0.0f, 1.0f}
{% endblock %}

{% block CameraUp %}
    {0.0f, 1.0f, 0.0f}
{% endblock %}

{% block LightPosition %}
    {
        (float)TextureWidth / 2.0f,
        100.0f,
        (float)TextureHeight / 2.0f
    }
{% endblock %}

{% block GenerateTerrainVertices %}
    for (auto z = 0; z < TextureHeight; ++z) {
        for (auto x = 0; x < TextureWidth; ++x) {
            Vertices.push_back({
                .Position = glm::vec3((float) x, 0.0f, (float) z),
                .TextureCoordinates = glm::vec2((float) -x / TextureWidth,
                                                (float) -z / TextureHeight)
            });
        }
    }
{% endblock GenerateTerrainVertices %}

{% block GenerateTerrainIndices %}
    for (auto i = 0; i < TextureHeight - 1; ++i) {
        for (auto j = 0; j < TextureWidth - 1; ++j) {
            auto Current = i * TextureWidth + j;
            auto Right = Current + 1;
            auto Bottom = Current + TextureWidth;
            auto BottomRight = Bottom + 1;

            Indices.push_back(Current);
            Indices.push_back(Right);
            Indices.push_back(Bottom);
            Indices.push_back(Right);
            Indices.push_back(BottomRight);
            Indices.push_back(Bottom);
        }
    }
{% endblock GenerateTerrainIndices %}
