#ifndef _DEFS_H_
#define _DEFS_H_

#include "Geometry.h"

using giraffe::vec3f;

class Scene;

struct HitRecord {
    float t;
    vec3f pos;
    vec3f normal;
    int materialIdx;
};

extern Scene *pScene;

#endif
