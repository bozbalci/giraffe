#include "Scaling.h"
#include "Matrix4.h"

#include <iomanip>
#include <iostream>

Scaling::Scaling() {}

Scaling::Scaling(int scalingId, double sx, double sy, double sz)
{
    this->scalingId = scalingId;
    this->sx = sx;
    this->sy = sy;
    this->sz = sz;
}

Matrix4 Scaling::getMatrix() const
{
    double mat[4][4] = {{sx, 0, 0, 0}, {0, sy, 0, 0}, {0, 0, sz, 0}, {0, 0, 0, 1}};
    return mat;
}

std::ostream &operator<<(std::ostream &os, const Scaling &s)
{
    os << std::fixed << std::setprecision(3) << "Scaling " << s.scalingId
       << " => [" << s.sx << ", " << s.sy << ", " << s.sz << "]";

    return os;
}
