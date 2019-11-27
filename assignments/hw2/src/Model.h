#ifndef __MODEL_H__
#define __MODEL_H__

#include "Triangle.h"
#include <iostream>
#include <vector>

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

    Model();
    Model(int modelId, int type, int numberOfTransformations,
          std::vector<int> transformationIds,
          std::vector<char> transformationTypes, int numberOfTriangles,
          std::vector<Triangle> triangles);

    friend std::ostream &operator<<(std::ostream &os, const Model &m);
};

#endif
