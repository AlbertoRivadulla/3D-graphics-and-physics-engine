#include "PhysicsBody.h"

using namespace GLGeometry;
using namespace GLBase;

namespace Physics
{
    //--------------------------------------------------------------------------
    // CollisionBody class

    // Constructor
    CollisionBody::CollisionBody()
    {

    }

    // Destructor
    CollisionBody::~CollisionBody()
    {
        delete mGeometryObject;
        delete mMaterial;
        // delete mCollider;
    }

    // // Add collider
    // void CollisionBody::addCollider( Collider* collider )
    // {
    //
    // }

    // Add geometrical object
    void CollisionBody::addGeometricalObject( GLElemObject* object )
    {
        mGeometryObject = object;
    }

    // Add material
    void CollisionBody::addMaterial( Material* material )
    {
        mMaterial = material;
    }

    // Draw
    void CollisionBody::draw( Shader& shader )
    {
        // Activate the shader
        shader.use();

        // Set the model matrix of the object and configure it in the shader
        //////////////////////////
        //////////////////////////
        mGeometryObject->setModelMatrix( glm::vec3(0., 5., 0.), 0., glm::vec3(1., 0., 0.), glm::vec3( 10., 10., 10. ) );
        //////////////////////////
        //////////////////////////
        shader.setMat4( "model", mGeometryObject->getModelMatrix() );

        // Configure the material in the shader
        mMaterial->configShader( shader );

        // Draw the object
        mGeometryObject->draw();
    }


    //--------------------------------------------------------------------------
    // RigidBody class

    // Constructor
    RigidBody::RigidBody()
    {

    }

}
