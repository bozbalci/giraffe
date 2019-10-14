#include "Ray.h"

Ray::Ray() {}

Ray::Ray(const vec3f &origin, const vec3f &direction)
    : origin(origin), direction(direction)
{
}

vec3f Ray::getPoint(float t) const { return origin + t * direction; }

/* Takes a point p and returns the parameter t according to p such that p =
 * o+t*d. */
float Ray::gett(const vec3f &p) const { throw; }
