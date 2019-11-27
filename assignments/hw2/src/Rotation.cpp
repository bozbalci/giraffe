#include "Rotation.h"
#include <iomanip>
#include <iostream>

Rotation::Rotation() {}

Rotation::Rotation(int rotationId, double angle, double x, double y, double z)
{
    this->rotationId = rotationId;
    this->angle = angle;
    this->ux = x;
    this->uy = y;
    this->uz = z;
}

std::ostream &operator<<(std::ostream &os, const Rotation &r)
{
    os << std::fixed << std::setprecision(3) << "Rotation " << r.rotationId
       << " => [angle=" << r.angle << ", " << r.ux << ", " << r.uy << ", "
       << r.uz << "]";

    return os;
}
