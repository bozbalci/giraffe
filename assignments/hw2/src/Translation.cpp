#include "Translation.h"
#include "Matrix4.h"

#include <iomanip>
#include <iostream>

Translation::Translation()
{
    this->translationId = -1;
    this->tx = 0.0;
    this->ty = 0.0;
    this->tz = 0.0;
}

Translation::Translation(int translationId, double tx, double ty, double tz)
{
    this->translationId = translationId;
    this->tx = tx;
    this->ty = ty;
    this->tz = tz;
}

Matrix4 Translation::getMatrix() const
{
    double mat[4][4] = {{1, 0, 0, tx}, {0, 1, 0, ty}, {0, 0, 1, tz}, {0, 0, 0, 1}};
    return mat;
}

std::ostream &operator<<(std::ostream &os, const Translation &t)
{
    os << std::fixed << std::setprecision(3) << "Translation "
       << t.translationId << " => [" << t.tx << ", " << t.ty << ", " << t.tz
       << "]";

    return os;
}
