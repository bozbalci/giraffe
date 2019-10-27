#include <limits>

#include "Ray.h"

Ray::Ray()
    : origin({std::numeric_limits<float>::max(), 0, 0}), direction({0, 0, 0})
{
    ;
}

Ray::Ray(const vec3f &origin, const vec3f &direction)
    : origin(origin), direction(direction)
{
    ;
}

vec3f Ray::getPoint(float t) const { return origin + t * direction; }

/* Takes a point p and returns the parameter t according to p such that p =
 * o+t*d. */
float Ray::gett(const vec3f &p) const { throw; }
