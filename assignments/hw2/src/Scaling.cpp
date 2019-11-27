#include "Scaling.h"
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

std::ostream &operator<<(std::ostream &os, const Scaling &s)
{
    os << std::fixed << std::setprecision(3) << "Scaling " << s.scalingId
       << " => [" << s.sx << ", " << s.sy << ", " << s.sz << "]";

    return os;
}
