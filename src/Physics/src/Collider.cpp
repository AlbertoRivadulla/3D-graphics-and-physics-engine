#include "Colliders.h"


using namespace GLBase;
using namespace GLGeometry;

namespace Physics
{
    //--------------------------------------------------------------------------
    // Collider class

    // Constructor
    Collider::Collider()
    {
        std::cout << "collider" << std::endl;

    }

    // Collider::~Collider()
    // {
    //     std::cout << "destr collider" << std::endl;
    // }

    // Method to check for a collision with another object's AABB
    bool Collider::checkCollisionAABB( const Collider* other ) const
    {
        return ( mAABB.cornersWorld[0].x <= other->mAABB.cornersWorld[1].x &&
                 mAABB.cornersWorld[1].x >= other->mAABB.cornersWorld[0].x &&
                 mAABB.cornersWorld[0].y <= other->mAABB.cornersWorld[1].y &&
                 mAABB.cornersWorld[1].y >= other->mAABB.cornersWorld[0].y &&
                 mAABB.cornersWorld[0].z <= other->mAABB.cornersWorld[1].z &&
                 mAABB.cornersWorld[1].z >= other->mAABB.cornersWorld[0].z );
    }

    // Method to check for a collision with a plane
    bool Collider::checkCollisionAABBPlane( const Collider* plane ) const
    {
        //
        //
        //
        //
        //
        //

        return true;
    }

    // Compute the eight vertices of the AABB in model space
    void Collider::computeVerticesAABB()
    {
        // Compute the eight vertices
        for ( int i = 0; i < 2; ++i )
            for ( int j = 0; j < 2; ++j )
                for ( int k = 0; k < 2; ++k )
                    mAABB.verticesModel.push_back( glm::vec4( mAABB.cornersModel[i][0],
                                                              mAABB.cornersModel[j][1],
                                                              mAABB.cornersModel[k][2],
                                                              1.f ) );

        // Initialize the two corners in world space
        mAABB.cornersWorld.push_back( glm::vec3( 0.f, 0.f, 0.f ) );
        mAABB.cornersWorld.push_back( glm::vec3( 0.f, 0.f, 0.f ) );
    }

    // Compute the AABB from a mesh of vertices, after a transformation
    void Collider::computeAABBTransformed( const std::vector<glm::vec4>& vertices,
                                           const glm::mat4& modelMatrix,
                                           std::vector<glm::vec3>& corners )
    {
        // Initialize minimum and maximum of each vertex
        for ( int i = 0; i < 3; ++i )
        {
            corners[0][i] = std::numeric_limits<float>::max();
            corners[1][i] = -std::numeric_limits<float>::max();
        }
        // Find the maximum and minimum values in each coordinate
        // Iterate through the vertices
        for ( auto vertex : vertices )
        {
            // Compute the transformed vertex
            glm::vec4 vertexTrans = modelMatrix * vertex;

            // Iterate through the 3 dimensions
            for ( int i = 0; i < 3; ++i )
            {
                if ( vertexTrans[i] < corners[0][i] )
                    corners[0][i] = vertexTrans[i];
                if ( vertexTrans[i] > corners[1][i] )
                    corners[1][i] = vertexTrans[i];
            }
        }
    }

};
