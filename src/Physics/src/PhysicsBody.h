#ifndef PHYSICS_BODY_H
#define PHYSICS_BODY_H

// #include "Physics.h"

#include "GLBase.h"
#include "GLGeometry.h"

using namespace GLGeometry;
using namespace GLBase;

namespace Physics
{
    // Struct that describes the collision points between two objects A and B
    struct CollisionPoints
    {
        glm::vec3 A;       // Furthest point of A into B
        glm::vec3 B;       // Furthest point of B into A
        glm::vec3 Normal;  // B – A normalized
        float Depth;       // Length of B – A
        bool HasCollision;
    };

    // Class for objects with collisions
    class CollisionBody
    {
        public:
            // Constructor
            CollisionBody();

            // Destructor
            ~CollisionBody();

            // // Add collider
            // void addCollider( Collider* collider );

            // Add geometrical object
            void addGeometricalObject( GLElemObject* object );

            // Add material
            void addMaterial( Material* material );

            // Draw
            void draw( Shader& shader );

        protected:
            // Position, scale and Rotation
            glm::vec3 mPosition;
            glm::vec3 mScale;
            glm::mat4 mRotation;

            // Collider
            // Collider* mCollider;

            // Material
            Material* mMaterial;

            // Geometrical object
            GLElemObject* mGeometryObject;
    };


    // Class for objects with both collisions and dynamics
    class RigidBody : public CollisionBody
    {
        public:
            // Constructor
            RigidBody();

            // Update transform

        private:
            // Variables for dynamics
            float mMass;
            glm::vec3 mVelocity;
            glm::vec3 mForce;
    };
}

#endif
