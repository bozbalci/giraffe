#ifndef __SCALING_H__
#define __SCALING_H__

#include <iostream>

class Scaling
{
  public:
    int scalingId;
    double sx, sy, sz;

    Scaling();
    Scaling(int scalingId, double sx, double sy, double sz);
    friend std::ostream &operator<<(std::ostream &os, const Scaling &s);
};

#endif
