#include "Rotation.h"
#include "Matrix4.h"

#include <iomanip>
#include <iostream>
#include <cmath>

Rotation::Rotation() {}

Rotation::Rotation(int rotationId, double angle, double x, double y, double z)
{
    this->rotationId = rotationId;
    this->angle = angle;
    this->ux = x;
    this->uy = y;
    this->uz = z;
}

Matrix4 Rotation::getMatrix() const
{
    using std::abs;

    double mat[4][4] = {0};
    if (abs(ux) < abs(uy) && abs(ux) < abs(uz)) {
        // ux is the smallest
    } else if (abs(u {
    }
}

std::ostream &operator<<(std::ostream &os, const Rotation &r)
{
    os << std::fixed << std::setprecision(3) << "Rotation " << r.rotationId
       << " => [angle=" << r.angle << ", " << r.ux << ", " << r.uy << ", "
       << r.uz << "]";

    return os;
}
