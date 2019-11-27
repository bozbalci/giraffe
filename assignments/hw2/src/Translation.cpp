#include "Translation.h"
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

std::ostream &operator<<(std::ostream &os, const Translation &t)
{
    os << std::fixed << std::setprecision(3) << "Translation "
       << t.translationId << " => [" << t.tx << ", " << t.ty << ", " << t.tz
       << "]";

    return os;
}
