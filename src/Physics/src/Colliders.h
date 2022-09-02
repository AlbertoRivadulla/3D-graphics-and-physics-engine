#ifndef COLLIDERS_H
#define COLLIDERS_H

#include "GLBase.h"
#include "GLGeometry.h"

using namespace GLBase;
using namespace GLGeometry;

namespace Physics
{
    // Axis aligned boundary box
    struct AABB
    {
        // Min and max vertices in model space
        std::vector<glm::vec3> cornersModel;

        // All vertices in model space
        std::vector<glm::vec4> verticesModel;

        // Min and max vertices in world space
        std::vector<glm::vec3> cornersWorld;
    };

    // Forward declare the different classes
    class SphereCollider;
    class PlaneCollider;
    class ConvexCollider;

    // Base collider class
    class Collider
    {
        public:
            // Constructor
            Collider();
            //
            // ~Collider();

            // Update the collider and AABB after a transformation
            // This needs to be implemented for each collider
            virtual void moveCollider( const glm::mat4& modelMatrix ) = 0;

            // Method to check for a collision with another object's AABB
            bool checkCollisionAABB( const Collider* other ) const;
            // Method to check for a collision with a plane
            bool checkCollisionAABBPlane( const Collider* plane ) const;

            // Methods to check for collisions with different colliders
            virtual bool findCollision( const Collider* other ) const = 0;
            virtual bool findCollision( const SphereCollider* other ) const = 0;
            virtual bool findCollision( const PlaneCollider* other ) const = 0;
            virtual bool findCollision( const ConvexCollider* other ) const = 0;

            // Set any other collider as a friend
            friend class Collider;

        protected:
            // Axis aligned boundary box, for the broad phase of collision checking
            AABB mAABB;

            // Compute the eight vertices of the AABB in model space
            void computeVerticesAABB();

            // Compute the AABB from a mesh of vertices, after a transformation
            void computeAABBTransformed( const std::vector<glm::vec4>& vertices,
                                         const glm::mat4& modelMatrix,
                                         std::vector<glm::vec3>& corners );
    };

    // Sphere collider
    class SphereCollider : public Collider
    {

        public:
            // Constructor
            // This initializes the radius to one
            // It also computes the AABB
            SphereCollider();

            // Update the collider and AABB after a transformation
            void moveCollider( const glm::mat4& modelMatrix );

            // Methods for finding collisions
            bool findCollision( const Collider* other ) const;
            bool findCollision( const SphereCollider* other ) const;
            bool findCollision( const PlaneCollider* other ) const;
            bool findCollision( const ConvexCollider* other ) const;

            friend class PlaneCollider;
            friend class ConvexCollider;

        private:
            // Radius of the sphere
            float mRadius;
            // Center of the sphere
            glm::vec3 mCenter;
    };

    // Plane collider
    class PlaneCollider : public Collider
    {
        public:
            // Constructor
            // This initializes the plane as being 1x1, centered at the origin
            // This also computes the AABB
            PlaneCollider();

            // Update the collider and AABB after a transformation
            void moveCollider( const glm::mat4& modelMatrix );

            // Methods for finding collisions
            bool findCollision( const Collider* other ) const;
            bool findCollision( const SphereCollider* other ) const;
            bool findCollision( const PlaneCollider* other ) const;
            bool findCollision( const ConvexCollider* other ) const;

            friend class SphereCollider;
            friend class ConvexCollider;

        private:
            // // Vectors of vertices, in model space and world space
            // std::vector<glm::vec3> mVerticesModel;
            // std::vector<glm::vec3> mVerticesWorld;

            // Position of the center
            glm::vec3 mCenter;
            // Normal vector
            glm::vec3 mNormal;
            // Tangent vectors
            std::vector<glm::vec3> mTangent;
            // Dimensions
            std::vector<float> mDimensions;
    };

    // Generic convex collider
    class ConvexCollider : public Collider
    {
        public:
            // Constructor with a GLElemObject as argument
            // This computes the AABB
            ConvexCollider( GLElemObject* elemObject );

            // Update the collider and AABB after a transformation
            void moveCollider( const glm::mat4& modelMatrix );

            // Methods for finding collisions
            bool findCollision( const Collider* other ) const;
            bool findCollision( const SphereCollider* other ) const;
            bool findCollision( const PlaneCollider* other ) const;
            bool findCollision( const ConvexCollider* other ) const;

            friend class SphereCollider;
            friend class PlaneCollider;

        private:
            // Vectors of vertices, in model space and world space
            std::vector<glm::vec3> mVerticesModel;
            std::vector<glm::vec3> mVerticesWorld;

            // Compute the AABB in model space from a vector of vertices
            void computeAABB( GLElemObject* elemObject );
    };

};

#endif
