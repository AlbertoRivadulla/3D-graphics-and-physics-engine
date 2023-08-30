#include "Colliders.h"

using namespace GLBase;
using namespace GLGeometry;

namespace Physics
{
    //--------------------------------------------------------------------------
    // ConvexCollider class

    // Constructor with a GLElemObject as argument
    ConvexCollider::ConvexCollider( GLElemObject* elemObject )
    {
        // std::cout << "convex collider" << std::endl;

        // Compute the AABB in model space, from the vertices of the GLElemObject
        mAABB.cornersModel.push_back( glm::vec3( -0.5f, -0.5f, -0.5f ) );
        mAABB.cornersModel.push_back( glm::vec3(  0.5f,  0.5f,  0.5f ) );

        // Compute the vertices of the AABB in model space
        computeAABB( elemObject );

        // Compute the vertices of the AABB in model space
        computeVerticesAABB();
    }
    
    // Compute the AABB in model space from a vector of vertices
    void ConvexCollider::computeAABB( GLElemObject* elemObject )
    {
        // Initialize minimum and maximum of each vertex
        mAABB.cornersModel.push_back( glm::vec3( 0.f, 0.f, 0.f ) );
        mAABB.cornersModel.push_back( glm::vec3( 0.f, 0.f, 0.f ) );
        for ( int i = 0; i < 3; ++i )
        {
            mAABB.cornersModel[0][i] = std::numeric_limits<float>::max();
            mAABB.cornersModel[1][i] = -std::numeric_limits<float>::max();
        }
        // Find the maximum and minimum values in each coordinate
        // Iterate through the vertices
        for ( auto vertex : elemObject->getVertices() )
        {
            // Iterate through the 3 dimensions
            for ( int i = 0; i < 3; ++i )
            {
                if ( vertex[i] < mAABB.cornersModel[0][i] )
                    mAABB.cornersModel[0][i] = vertex[i];
                if ( vertex[i] > mAABB.cornersModel[1][i] )
                    mAABB.cornersModel[1][i] = vertex[i];
            }
        }
    }

    // Update the collider and AABB after a transformation
    void ConvexCollider::moveCollider( const glm::mat4& modelMatrix )
    {
        // std::cout << "moving Convex" << std::endl;

        // Update the AABB
        computeAABBTransformed( mAABB.verticesModel, modelMatrix, mAABB.cornersWorld );

        // Update the collider

    }

    // Methods for finding collisions
    bool ConvexCollider::findCollision( const Collider* other ) const
    {
        return other->findCollision( this );
    }

    bool ConvexCollider::findCollision( const SphereCollider* sphere ) const
    {
        // Test collisions between AABBs
        if ( checkCollisionAABB( sphere ) )
        {
            std::cout << "Collision convex sphere" << std::endl;
            return true;
        }

        //
        //
        //

        return true;
    }

    bool ConvexCollider::findCollision( const PlaneCollider* plane ) const
    {
        // Test collision with the AABB
        if ( checkCollisionAABBPlane( plane ) )
        {
            return true;

        }
        // std::cout << "convex plane" << std::endl;

        //
        //
        //

        return true;
    }

    bool ConvexCollider::findCollision( const ConvexCollider* other ) const
    {
        // Test collisions between AABBs
        if ( checkCollisionAABB( other ) )
        {
            std::cout << "Collision convex convex" << std::endl;
            return true;
        }

        //
        //
        //

        return true;
    }


}
