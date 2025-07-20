#include "Colliders.h"

using namespace GLBase;
using namespace GLGeometry;

namespace Physics
{
    //--------------------------------------------------------------------------
    // SphereCollider class

    // Constructor
    SphereCollider::SphereCollider() : 
        mRadius { 1.f }
    {
        // Compute the AABB in model space
        mAABB.cornersModel.push_back( glm::vec3( -0.5f, -0.5f, -0.5f ) );
        mAABB.cornersModel.push_back( glm::vec3(  0.5f,  0.5f,  0.5f ) );

        // Compute the vertices of the AABB in model space
        computeVerticesAABB();

        // Place the center at the position ( 0, 0, 0 )
        mCenter = glm::vec3( 0., 0., 0. );
    }

    // Update the collider and AABB after a transformation
    void SphereCollider::moveCollider( const glm::mat4& modelMatrix )
    {
        // Update the AABB
        computeAABBTransformed( mAABB.verticesModel, modelMatrix, mAABB.cornersWorld );

        // Move the center
        mCenter = glm::vec3( modelMatrix[3][0],
                             modelMatrix[3][1],
                             modelMatrix[3][2] );

        // Change the radius using the given model matrix
        mRadius = 0.f;
        for ( int i = 0; i < 3; ++i )
            mRadius += modelMatrix[i][0] * modelMatrix[i][0];
        mRadius = 0.5f * std::sqrt( mRadius );
    }

    // Methods for finding collisions
    bool SphereCollider::findCollision( const Collider* other ) const
    {
        return other->findCollision( this );
    }

    bool SphereCollider::findCollision( const SphereCollider* other ) const
    {

        // Test collisions between AABBs first
        if ( checkCollisionAABB( other ) )
        {
            // Compute the sum of the radii of the spheres squared
            float sumRadiusSq = ( mRadius + other->mRadius ) * 
                                ( mRadius + other->mRadius );

            // Compute the distance squared between the centers of the spheres
            float distSq = 0.f;
            glm::vec3 separation = mCenter - other->mCenter;
            for ( int i = 0; i < 3; ++i )
                distSq += separation[ i ] * separation[ i ];

            // Compare the two quantities
            return distSq <= sumRadiusSq;
        }

        return false;
    }

    bool SphereCollider::findCollision( const PlaneCollider* plane ) const
    {
        // Test collision with the AABB
        if ( checkCollisionAABBPlane( plane ) )
        {
            // Vector between centers
            glm::vec3 vCenters = mCenter - plane->mCenter;
            // Projection of the center of the sphere on the three axes of the plane
            glm::vec3 projections;

            // Project on the tangent directions
            projections[ 0 ] = glm::dot( vCenters, plane->mTangent[ 0 ] );
            projections[ 1 ] = glm::dot( vCenters, plane->mTangent[ 1 ] );

            // Check if these projections fall inside the plane
            if ( projections[ 0 ] + mRadius <= plane->mDimensions[ 0 ] &&
                 projections[ 1 ] + mRadius <= plane->mDimensions[ 1 ] )
            {

                // Project the center on the normal
                projections[ 2 ] = glm::dot( vCenters, plane->mNormal );

                // Check if the sphere lies entirely on the plane
                if ( projections[ 0 ] <= plane->mDimensions[ 0 ] &&
                     projections[ 1 ] <= plane->mDimensions[ 1 ] &&
                     std::fabs( projections[ 2 ] ) <= mRadius )
                {
                    return true;
                }

                // Otherwise, the computation is more complex, and this needs to be 
                // implemented
                //
                //
                //
            }
        }

        return false;
    }

    bool SphereCollider::findCollision( const ConvexCollider* other ) const
    {
        // Tbis is implemented in the class ConvexCollider
        return other->findCollision( this );
    }
}
