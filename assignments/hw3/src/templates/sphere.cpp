{% extends "__base.cpp" %}

{% block Variant %}sphere{% endblock %}
{% block VertexShaderPath %}shaders/sphere.vert{% endblock %}
{% block FragmentShaderPath %}shaders/sphere.frag{% endblock %}
{% block HeightFactorInitial %}0.0f{% endblock %}

{% block CameraPosition %}
    {0.0f, 600.0f, 0.0f}
{% endblock %}

{% block CameraGaze %}
    {0.0f, -1.0f, 0.0f}
{% endblock %}

{% block CameraUp %}
    {0.0f, 0.0f, 1.0f}
{% endblock %}

{% block LightPosition %}
    {0.0f, 1600.0f, 0.0f}
{% endblock %}

{% block GenerateTerrainVertices %}
    auto Radius = 350;
    auto NumberOfLongitudeLines = 250;
    auto NumberOfLatitudeLines = 125;

    auto LongitudeAngleStep = PI / NumberOfLongitudeLines;
    auto LatitudeAngleStep = 2 * PI / NumberOfLatitudeLines;

    for (auto i = 0; i <= NumberOfLongitudeLines; ++i) {
        auto LongitudeAngle = (PI / 2) - (i * LongitudeAngleStep);
        auto XYComponent = Radius * std::cos(LongitudeAngle);
        auto ZComponent = Radius * std::sin(LongitudeAngle);

        for (auto j = 0; j <= NumberOfLatitudeLines; ++j) {
            auto LatitudeAngle = j * LatitudeAngleStep;

            Vertices.push_back({
                .Position = {
                    XYComponent * std::cos(LatitudeAngle),
                    XYComponent * std::sin(LatitudeAngle),
                    ZComponent
                },
                .TextureCoordinates = {
                    (float)j / NumberOfLatitudeLines,
                    (float)i / NumberOfLongitudeLines
                }
            });
        }
    }
{% endblock GenerateTerrainVertices %}

{% block GenerateTerrainIndices %}
    int k1, k2;

    auto NumberOfLongitudeLines = 250;
    auto NumberOfLatitudeLines = 125;

    for (int i = 0; i < NumberOfLongitudeLines; ++i) {
        k1 = i * (NumberOfLatitudeLines + 1);
        k2 = k1 + NumberOfLatitudeLines + 1;

        for (int j = 0; j < NumberOfLatitudeLines; ++j) {
            if (i != 0) {
                Indices.push_back(k1);
                Indices.push_back(k2);
                Indices.push_back(k1 + 1);
            }
            if (i != NumberOfLongitudeLines - 1) {
                Indices.push_back(k1 + 1);
                Indices.push_back(k2);
                Indices.push_back(k2 + 1);
            }

            ++k1;
            ++k2;
        }
    }
{% endblock GenerateTerrainIndices %}
