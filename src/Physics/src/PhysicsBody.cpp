#include "PhysicsBody.h"

using namespace GLGeometry;
using namespace GLBase;

namespace Physics
{
    //--------------------------------------------------------------------------
    // CollisionBody class

    // Constructor
    CollisionBody::CollisionBody( glm::vec3 position, glm::vec3 scale,
                                  float rotationAngle, glm::vec3 rotationAxis ) :
        mPosition { position }, mScale { scale }
    {
        // Compute the rotation matrix from the angle and axis given
        mRotationMatrix = glm::mat4( 1.f );
       std::cout << rotationAngle << std::endl;
        if (rotationAngle != 0.)
            mRotationMatrix = glm::rotate( mRotationMatrix, glm::radians(rotationAngle), 
                                           glm::normalize(rotationAxis) );
    }

    // Destructor
    CollisionBody::~CollisionBody()
    {
        // The variable mGeometryObject is already deleted by the destructor
        // delete mGeometryObject;


        delete mMaterial;
        // delete mCollider;
    }

    // // Add collider
    // void CollisionBody::addCollider( Collider* collider )
    // {
    //
    // }

    // Add geometrical object, and copy it to the list of elementary objects of
    // the GLSandbox class
    void CollisionBody::addGeometry( GLElemObject* objectPtr, 
                                     std::vector<GLElemObject*>& elemObjs )
    {
        mGeometryObject = objectPtr;
        elemObjs.push_back( objectPtr );
    }

    // Add material
    void CollisionBody::addMaterial( Material* material )
    {
        mMaterial = material;
    }

    // Set position
    void CollisionBody::setPosition( glm::vec3 position )
    {
        mPosition = position;
    }

    // Set Scale
    void CollisionBody::setScale( glm::vec3 scale )
    {
        mScale = scale;
    }

    // Draw
    void CollisionBody::draw( Shader& shader )
    {
        // Activate the shader
        shader.use();

        // Set the model matrix of the object and configure it in the shader
        //////////////////////////
        //////////////////////////
        // mGeometryObject->setModelMatrix( mPosition, 0., glm::vec3(1., 0., 0.), mScale );
        mGeometryObject->setModelMatrix( mPosition, mRotationMatrix, mScale );
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
    RigidBody::RigidBody( glm::vec3 position, glm::vec3 scale, 
                          float rotationAngle, glm::vec3 rotationAxis,
                          float mass, glm::vec3 velocity ) :
        CollisionBody( position, scale, rotationAngle, rotationAxis ),      // Initialize the base class explicitly
        mVelocity { velocity }, mMass { mass }, mMassInver { 1.f / mass },
        mAngularVelocity { glm::vec3( 0.f, 0.f, 0.f ) },
        mForce { glm::vec3( 0.f, 0.f, 0.f ) }, 
        mTorque { glm::vec3( 0.f, 0.f, 0.f ) },
        mGravity { glm::vec3( 0.f, -9.8f, 0.f ) }
    {

    }

    // Set velocity
    void RigidBody::setVelocity( glm::vec3 velocity )
    {
        mVelocity = velocity;
    }

    // Set mass 
    void RigidBody::setMass( float mass )
    {
        mMass = mass;
        mMassInver = 1.f / mass;
    }

    // Update movement
    void RigidBody::updateMovement( float deltaTime )
    {
        // Update velocity, adding the acceleration from the force and the gravity
        mVelocity += ( mForce * mMassInver + mGravity ) * deltaTime;
        // Update angular velocity
        mAngularVelocity += mTorque * mMassInver * deltaTime;

        // Update the position
        mPosition += mVelocity * deltaTime;

        // Compute the rotation angle and axis
        float angVelLength = glm::length( mAngularVelocity );
        float rotationAngle = RAD_TO_DEG * angVelLength * deltaTime;
        // Update the rotation
        mRotationMatrix = glm::rotate( mRotationMatrix, rotationAngle, 
                                       mAngularVelocity / angVelLength );

        // Reset the net force and torque on the object
        mForce = glm::vec3( 0.f, 0.f, 0.f );
        mTorque = glm::vec3( 0.f, 0.f, 0.f );
    }

}
