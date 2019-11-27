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
    this->u = normalizeVec3({x, y, z});
}

Matrix4 Rotation::getMatrix() const
{
    using std::abs;

    Vec3f v;

    if (abs(u.x) <= abs(u.y) && abs(u.x) <= abs(u.z)) {
        // ux is the smallest
        v.x = 0;
        v.y = -u.z;
        v.z = u.y;
    } else if (abs(u.y) <= abs(u.x) && abs(u.y) <= abs(u.z)) {
        // uy is the smallest
        v.x = -u.z;
        v.y = 0;
        v.z = u.y;
    } else {
        // uz is the smallest
        v.x = -u.y;
        v.y = u.x;
        v.z = 0;
    }

    v = normalizeVec3(v);
    Vec3f w = crossProductVec3(u, v);

    double mat_[4][4] = {
        {u.x, u.y, u.z, 0.},
        {v.x, v.y, v.z, 0.},
        {w.x, w.y, w.z, 0.},
        {0., 0., 0., 1.}
    };

    double matInverse_[4][4] = {
        {u.x, v.x, w.x, 0.},
        {u.y, v.y, w.y, 0.},
        {u.z, v.z, w.z, 0.},
        {0., 0., 0., 1.}
    };

    double rx_[4][4] = {
        {1., 0., 0., 0.},
        {0., cos(angle), -sin(angle), 0.},
        {0., sin(angle), cos(angle), 0.},
        {0., 0., 0., 1.}
    }

    Matrix4f mat(mat_);
    Matrix4f matInverse(matInverse_);
    Matrix4f rx(rx_);

    return multiplyMatrixWithMatrix(
        multiplyMatrixWithMatrix(
            matInverse,
            rx,
        ),
        mat,
    );
}

std::ostream &operator<<(std::ostream &os, const Rotation &r)
{
    os << std::fixed << std::setprecision(3) << "Rotation " << r.rotationId
       << " => [angle=" << r.angle << ", " << r.u.x << ", " << r.u.y << ", "
       << r.u.z << "]";

    return os;
}
