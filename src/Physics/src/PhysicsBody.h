#ifndef PHYSICS_BODY_H
#define PHYSICS_BODY_H

// #include "Physics.h"

// #include "ForceGenerator.h"
#include "GLBase.h"
#include "GLGeometry.h"
#include "Colliders.h"

using namespace GLGeometry;
using namespace GLBase;

namespace Physics
{
    // Constants
    const float RAD_TO_DEG = 180.f / M_PI;

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
            // Collider
            Collider* mCollider;

            // Model matrix 
            glm::mat4 mModelMatrix;

            // Constructor
            // CollisionBody( glm::vec3 position, glm::vec3 scale );
            CollisionBody( glm::vec3 position, glm::vec3 scale,
                           float rotationAngle, glm::vec3 rotationAxis );

            // Destructor
            ~CollisionBody();

            // Add geometrical object, and copy it to the list of elementary objects of
            // the GLSandbox class
            void addGeometry( GLElemObject* objectPtr, 
                              std::vector<GLElemObject*>& elemObjs );

            // Add collider
            void addCollider( Collider* collider );

            // Add material
            void addMaterial( Material* material );

            // Set position
            void setPosition( glm::vec3 position );

            // Set Scale
            void setScale( glm::vec3 scale );

            // Set rotation
            void setRotation( float angle, glm::vec3 axis );

            // Method to compute a model matrix 
            void computeModelMatrix();

            // Draw
            void draw();

        protected:
            // Position, scale and Rotation
            glm::vec3 mPosition;
            glm::vec3 mScale;
            glm::mat4 mRotationMatrix;

            // Geometrical object
            GLElemObject* mGeometryObject;

            // Material
            Material* mMaterial;
    };


    // Class for objects with both collisions and dynamics
    class RigidBody : public CollisionBody
    {
        public:
            // Constructor
            RigidBody( glm::vec3 position, glm::vec3 scale,
                       float rotationAngle, glm::vec3 rotationAxis,
                       float mass, glm::vec3 velocity = {0.f, 0.f, 0.f} );

            // Set velocity and acceleration
            void setVelocity( glm::vec3 velocity );

            // Set mass 
            void setMass( float mass );
            void setInvMass( float invMass );

            // Set velocity damping
            void setDamping( float damping );

            // Getters
            float getMass();
            glm::vec3 getVelocity();

            // Check if it has infinite mass
            bool hasInfiniteMass();

            // Add a force
            void addForce( const glm::vec3& force );

            // Integrate forward in time by the given duration
            void integrate( float deltaTime );

        protected:
            // Variables for dynamics
            float mMass;
            float mMassInver;
            glm::vec3 mVelocity;
            // glm::vec3 mAcceleration;

            // Damping applied to linear motion, to ensure objects are not accelerated
            // due to numerical inaccuracies
            float mDamping;

            // Accumulator for forces
            glm::vec3 mForceAccum;
            // glm::vec3 mTorqueAccum;


            // // --------------------
            //
            // glm::vec3 mAngularVelocity;
            // glm::vec3 mForce;
            // glm::vec3 mTorque;

    };
}

#endif
