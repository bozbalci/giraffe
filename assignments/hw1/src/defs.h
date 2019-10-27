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
    int blipIdx = 0;
    int blopIdx = 0;
};

constexpr HitRecord NO_HIT = {-1, {0, 0, 0}, {0, 0, 0}, -1};

extern Scene *pScene;

#endif
