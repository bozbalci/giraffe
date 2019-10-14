#include "Ray.h"

Ray::Ray() {}

Ray::Ray(const vec3f &origin, const vec3f &direction)
    : origin(origin), direction(direction)
{
}

/* Takes a parameter t and returns the point accoring to t. t is the parametric
 * variable in the ray equation o+t*d.*/
vec3f Ray::getPoint(float t) const
{
    /***********************************************
     *                                             *
     * TODO: Implement this function               *
     *                                             *
     ***********************************************
     */
}

/* Takes a point p and returns the parameter t according to p such that p =
 * o+t*d. */
float Ray::gett(const vec3f &p) const
{
    /***********************************************
     *                                             *
     * TODO: Implement this function               *
     *                                             *
     ***********************************************
     */
}
