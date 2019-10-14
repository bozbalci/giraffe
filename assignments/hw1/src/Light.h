#ifndef _LIGHT_H_
#define _LIGHT_H_

#include "defs.h"

class PointLight
{
  public:
    vec3f position;

    PointLight(const vec3f &position, const vec3f &intensity);
    vec3f computeLightContribution(const vec3f &p);

  private:
    vec3f intensity;
};

#endif
