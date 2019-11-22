#include "Rotation.h"
#include <iomanip>
#include <iostream>

using namespace std;

Rotation::Rotation() {}

Rotation::Rotation(int rotationId, double angle, double x, double y, double z)
{
    this->rotationId = rotationId;
    this->angle = angle;
    this->ux = ux;
    this->uy = uy;
    this->uz = uz;
}

ostream &operator<<(ostream &os, const Rotation &r)
{
    os << fixed << setprecision(3) << "Rotation " << r.rotationId
       << " => [angle=" << r.angle << ", " << r.ux << ", " << r.uy << ", "
       << r.uz << "]";

    return os;
}