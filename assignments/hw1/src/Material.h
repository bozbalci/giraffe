#ifndef _MATERIAL_H_
#define _MATERIAL_H_

#include "defs.h"

class Material
{
  public:
    int id;
    int phongExp;
    vec3f ambientRef;
    vec3f diffuseRef;
    vec3f specularRef;
    vec3f mirrorRef;

    Material(void);
};

#endif
