#include "Geometry.h"

#include <cmath>

namespace giraffe
{

float vec3f::norm() const { return std::sqrt(dot(*this, *this)); }

vec3f vec3f::normalize() const
{
    float size = this->norm();

    return {this->x / size, this->y / size, this->z / size};
}

vec3f operator-(vec3f vec) { return {-vec.x, -vec.y, -vec.z}; }

vec3f operator+(vec3f lhs, vec3f rhs)
{
    return {lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z};
}

vec3f operator-(vec3f lhs, vec3f rhs)
{
    return {lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z};
}

vec3f operator*(float lhs, vec3f rhs)
{
    return {lhs * rhs.x, lhs * rhs.y, lhs * rhs.z};
}

vec3f operator/(vec3f lhs, float rhs)
{
    return {lhs.x / rhs, lhs.y / rhs, lhs.z / rhs};
}

float operator*(vec3f lhs, vec3f rhs) { return dot(lhs, rhs); }

float dot(vec3f lhs, vec3f rhs)
{
    return (lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z);
}

vec3f cross(vec3f lhs, vec3f rhs)
{
    return {lhs.y * rhs.z - lhs.z * rhs.y, lhs.z * rhs.x - lhs.x * rhs.z,
            lhs.x * rhs.y - lhs.y * rhs.x};
}

vec3f oymak(vec3f lhs, vec3f rhs)
{
    return {lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z};
}

} // namespace giraffe
