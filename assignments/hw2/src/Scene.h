#ifndef _SCENE_H_
#define _SCENE_H_

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>
#include <vector>

#include "Camera.h"
#include "Color.h"
#include "Model.h"
#include "Rotation.h"
#include "Scaling.h"
#include "Translation.h"
#include "Triangle.h"
#include "Vec3.h"
#include "Vec4.h"

class Scene
{
  public:
    Color backgroundColor;
    bool cullingEnabled;
    int projectionType;

    std::vector<std::vector<Color>> image;
    std::vector<Camera *> cameras;
    std::vector<Vec3 *> vertices;
    std::vector<Color *> colorsOfVertices;
    std::vector<Scaling *> scalings;
    std::vector<Rotation *> rotations;
    std::vector<Translation *> translations;
    std::vector<Model *> models;

    Scene(const char *xmlPath);

    void initializeImage(Camera *camera);
    void forwardRenderingPipeline(Camera *camera);
    int makeBetweenZeroAnd255(double value);
    void writeImageToPPMFile(Camera *camera);
    void convertPPMToPNG(std::string ppmFileName, int osType);
};

#endif
