#ifndef UTILS_H
#define UTILS_H

#include "GLBase.h"

namespace GLUtils
{
    // Linear interpolation between two values
    template <typename T>
    inline T lerp(const T& a, const T& b, float t)
    {
        return (1.f - t) * a + t * b;
    }

    // Get a randum number between 0 and 1
    inline float getRandom0To1()
    {
        return (float)( std::rand() / (float)RAND_MAX );
    }

    // Seed the random number generator with the clock
    inline void seedRandomGeneratorClock()
    {
        std::srand(static_cast<unsigned int>(std::time(nullptr))); 
    }
}

#endif
