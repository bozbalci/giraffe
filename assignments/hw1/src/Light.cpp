#include <cmath>

#include "Light.h"

PointLight::PointLight(const vec3f &position, const vec3f &intensity)
    : position(position), intensity(intensity)
{
}

vec3f PointLight::computeLightContribution(const vec3f &p)
{
    return intensity / std::pow((p - position).norm(), 2);
}
