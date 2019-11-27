#ifndef __ROTATION_H__
#define __ROTATION_H__

#include "Matrix4.h"

#include <iostream>

class Rotation
{
  public:
    int rotationId;
    double angle, ux, uy, uz;

    Matrix4 getMatrix() const;

    Rotation();
    Rotation(int rotationId, double angle, double x, double y, double z);
    friend std::ostream &operator<<(std::ostream &os, const Rotation &r);
};

#endif
