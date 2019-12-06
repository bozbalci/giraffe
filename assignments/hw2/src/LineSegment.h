#ifndef HW2_LINESEGMENT_H
#define HW2_LINESEGMENT_H


#include "Vec4.h"

class LineSegment {
public:
    Vec4 a, b;
    bool visible;

    LineSegment(const Vec4& a, const Vec4& b) : a(a), b(b), visible(false) {}
    void clip();
};


#endif //HW2_LINESEGMENT_H
