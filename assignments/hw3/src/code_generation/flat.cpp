{% extends "__base.cpp" %}

{% block Variant %}flat{% endblock %}
{% block VertexShaderPath %}shader.vert{% endblock %}
{% block FragmentShaderPath %}shader.frag{% endblock %}

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
