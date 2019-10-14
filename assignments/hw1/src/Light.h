#ifndef _LIGHT_H_
#define _LIGHT_H_

#include "defs.h"

// Class for point lights
class PointLight
{
  public:
    vec3f position; // Position of the point light

    PointLight(const vec3f &position,
               const vec3f &intensity); // Constructor
    vec3f computeLightContribution(
        const vec3f &p); // Compute the contribution of light at point p

  private:
    vec3f intensity; // Intensity of the point light
};

#endif
