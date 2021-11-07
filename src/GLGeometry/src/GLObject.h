#ifndef GLOBJECT_H
#define GLOBJECT_H

#include "GLGeometry.h"

namespace GLGeometry
{
    class GLObject
    {
        public:
            GLObject() {};
            // Function to render
            virtual void draw() = 0;

    };
}

#endif
