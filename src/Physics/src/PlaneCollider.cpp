#include "Colliders.h"

using namespace GLBase;
using namespace GLGeometry;

namespace Physics
{
    //--------------------------------------------------------------------------
    // PlaneCollider class

    // Constructor
    PlaneCollider::PlaneCollider()
    {
        std::cout << "plane collider" << std::endl;

        // Compute the AABB in model space
        mAABB.cornersModel.push_back( glm::vec3( -0.5f, 0.f, -0.5f ) );
        mAABB.cornersModel.push_back( glm::vec3(  0.5f, 0.f,  0.5f ) );

        // Compute the vertices of the AABB in model space
        computeVerticesAABB();
    }

    // Update the collider and AABB after a transformation
    void PlaneCollider::moveCollider( const glm::mat4& modelMatrix )
    {
        // Update the AABB
        computeAABBTransformed( mAABB.verticesModel, modelMatrix, mAABB.cornersWorld );

        // Move the center
        mCenter = glm::vec3( modelMatrix[3][0],
                             modelMatrix[3][1],
                             modelMatrix[3][2] );

        // Compute the normal vector
        mNormal = glm::vec3( glm::normalize( modelMatrix * glm::vec4( 0.f, 0.f, 1.f, 0.f ) ) );

        // Compute the two tangent vectors
        mTangent.push_back( glm::normalize( glm::vec3( modelMatrix * glm::vec4( 1.f, 0.f, 0.f, 0.f ) ) ) );
        mTangent.push_back( glm::normalize( glm::vec3( modelMatrix * glm::vec4( 0.f, 1.f, 0.f, 0.f ) ) ) );

        // Compute the dimensions
        for ( int j = 0; j < 2; ++j )
        {
            mDimensions.push_back( 0.f );
            for ( int i = 0; i < 3; ++i )
                mDimensions[ j ] += modelMatrix[i][j] * modelMatrix[i][j];
            mDimensions[ j ] = 0.5f * std::sqrt( mDimensions[ j ] );
        }
    }

    // Methods for finding collisions
    bool PlaneCollider::findCollision( const Collider* other ) const
    {
        return other->findCollision( this );
    }

    bool PlaneCollider::findCollision( const SphereCollider* other ) const
    {
        // This collision is implemented in the class SphereCollider
        return other->findCollision( this );
    }

    bool PlaneCollider::findCollision( const PlaneCollider* other ) const
    {
        // No collision between planes
        // return {};
        return false;
    }

    bool PlaneCollider::findCollision( const ConvexCollider* other ) const
    {
        // This collision is implemented in the class ConvexCollider
        return other->findCollision( this );
    }
}
