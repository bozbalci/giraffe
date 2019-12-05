#ifndef __MODEL_H__
#define __MODEL_H__

#include "Color.h"
#include "Vec4.h"
#include "Matrix4.h"
#include "Triangle.h"

#include <iostream>
#include <vector>
#include <optional>

class Model
{
  public:
    int modelId;
    int type;
    int numberOfTransformations;
    std::vector<int> transformationIds;
    std::vector<char> transformationTypes;
    int numberOfTriangles;
    std::vector<Triangle> triangles;

    std::optional<Matrix4> compositeTransform;
    std::vector<Vec4> transformedVertices;
    std::vector<Color> transformedColors;

    Model();
    Model(int modelId, int type, int numberOfTransformations,
          std::vector<int> transformationIds,
          std::vector<char> transformationTypes, int numberOfTriangles,
          std::vector<Triangle> triangles);

    friend std::ostream &operator<<(std::ostream &os, const Model &m);
};

#endif
