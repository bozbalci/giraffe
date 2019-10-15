#ifndef _MATERIAL_H_
#define _MATERIAL_H_

#include "defs.h"

class Material
{
  public:
    int id;
    /*
     * Some materials in some scenes do not have a PhongExponent defined.
     * This leads to phongExp remaining uninitialized and results in nasal
     * demons. Defaulting this to 0 should fix this.
     */
    int phongExp = 0;
    vec3f ambientRef;
    vec3f diffuseRef;
    vec3f specularRef;
    vec3f mirrorRef;

    Material(void);
};

#endif
