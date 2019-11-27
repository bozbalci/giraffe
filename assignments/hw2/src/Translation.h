#ifndef __TRANSLATION_H__
#define __TRANSLATION_H__

#include "Matrix4.h"

#include <iostream>

class Translation
{
  public:
    int translationId;
    double tx, ty, tz;

    Matrix4 getMatrix() const;

    Translation();
    Translation(int translationId, double tx, double ty, double tz);
    friend std::ostream &operator<<(std::ostream &os, const Translation &t);
};

#endif
