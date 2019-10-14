#ifndef _RAY_H_
#define _RAY_H_

#include "defs.h"

// Class for Ray object. You will implement the methods
class Ray
{
  public:
    vec3f origin;    // Origin of the ray
    vec3f direction; // Direction of the ray

    Ray();                                            // Constuctor
    Ray(const vec3f &origin, const vec3f &direction); // Constuctor

    vec3f getPoint(
        float t) const; // Return the point along the ray at ray parameter t
    float gett(const vec3f &p)
        const; // Return the t parameter at point p along the ray

  private:
    // Write any other stuff here.
};

#endif
