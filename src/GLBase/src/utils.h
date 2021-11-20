#ifndef UTILS_H
#define UTILS_H

#include "GLBase.h"

namespace GLUtils
{
    // Linear interpolation between two values
    template <typename T>
    T lerp(const T& a, const T& b, float t)
    {
        return (1.f - t) * a + t * b;
    }

}

#endif
