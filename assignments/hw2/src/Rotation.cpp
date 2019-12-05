#include "Rotation.h"

#include "Helpers.h"
#include "Matrix4.h"
#include "Vec3.h"

#include <iomanip>
#include <iostream>
#include <cmath>

Rotation::Rotation() {}

Rotation::Rotation(int rotationId, double angle, double x, double y, double z)
{
    this->rotationId = rotationId;
    this->angle = angle;
    this->u = normalizeVec3(Vec3(x, y, z, /* colorId = */ 0));
}

Matrix4 Rotation::getMatrix() const
{
    const double pi = std::acos(-1);
    auto cos = std::cos(angle * (pi/180));
    auto sin = std::sin(angle * (pi/180));
    auto mcos = 1 - cos;

    double mat[4][4] = {
            {cos + u.x * u.x * mcos,       u.x * u.y * mcos - u.z * sin, u.x * u.z * mcos + u.y * sin, 0.},
            {u.y * u.x * mcos + u.z * sin, cos + u.y * u.y * mcos,       u.y * u.z * mcos - u.x * sin, 0.},
            {u.z * u.x * mcos - u.y * sin, u.z * u.y * mcos + u.x * sin, cos + u.z * u.z * mcos,       0.},
            {0.,                           0.,                           0.,                           1.}
    };

    return mat;
}

std::ostream &operator<<(std::ostream &os, const Rotation &r)
{
    os << std::fixed << std::setprecision(3) << "Rotation " << r.rotationId
       << " => [angle=" << r.angle << ", " << r.u.x << ", " << r.u.y << ", "
       << r.u.z << "]";

    return os;
}
