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

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/norm.hpp>
#include <glm/gtx/string_cast.hpp>

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

    inline void printMatrix4( glm::mat4 matrix )
    {
        // std::cout << "\n";
        for ( int i = 0; i < 4; ++i )
        {
            std::cout << "[ ";
            for ( int j = 0; j < 4; ++j )
                std::cout << matrix[i][j] << ' ';
            std::cout << "]\n";
        }
        std::cout << "\n";
    }

    inline void printVec3( glm::vec3 vector )
    {
        std::cout << "( ";
        for ( int i = 0; i < 3; ++i )
            std::cout << vector[i] << ' ';
        std::cout << ")\n";
    }

    inline void printVec4( glm::vec4 vector )
    {
        std::cout << "( ";
        for ( int i = 0; i < 4; ++i )
            std::cout << vector[i] << ' ';
        std::cout << ")\n";
    }
}

#endif
