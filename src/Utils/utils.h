#ifndef UTILS_H
#define UTILS_H

#include <iostream>
#include <cstdlib>
#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <cmath>
#include <map>
#include <random>
#include <ctime>
#include <list>
#include <iterator>

#include "src/logger.h"

namespace Utils
{
    template <typename T>
    inline T lerp(const T& a, const T& b, float t)
    {
        return (1.f - t) * a + t * b;
    }

    inline float getRandom0To1()
    {
        return (float)( std::rand() / (float)RAND_MAX );
    }

    inline void seedRandomGeneratorClock()
    {
        std::srand(static_cast<unsigned int>(std::time(nullptr))); 
    }
}

#endif
