#include "PhysicsBody.h"
#include "utils.h"

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
        if (rotationAngle != 0.)
            mRotationMatrix = glm::rotate( mRotationMatrix, glm::radians(rotationAngle), 
                                           glm::normalize(rotationAxis) );
    }

    // Destructor
    CollisionBody::~CollisionBody()
    {
        // The variable mGeometryObject is already deleted by the destructor

        delete mMaterial;
        // delete mCollider;
    }

    // Add geometrical object, and copy it to the list of elementary objects of
    // the GLSandbox class
    void CollisionBody::addGeometry( GLElemObject* objectPtr, 
                                     std::vector<GLElemObject*>& elemObjs )
    {
        // Store the pointer to the geometry as a member object
        mGeometryObject = objectPtr;
        // Store the pointer also in the list of elementary objects in the scene
        elemObjs.push_back( objectPtr );

        // Compute the model matrix of the geometry object
        computeModelMatrix();
    }

    // Add collider
    void CollisionBody::addCollider( Collider* collider )
    {
        //
        //
        //

        // Store the collider
        mCollider = collider;

        // Pass the transformation matrix to the collider
        mCollider->moveCollider( mModelMatrix );

        //
        //
        //
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

        // Update the model matrix of the geometry object
        // mModelMatrix = computeModelMatrix( mPosition, mRotationMatrix, mScale );
        computeModelMatrix();
    }

    // Set Scale
    void CollisionBody::setScale( glm::vec3 scale )
    {
        mScale = scale;

        // Update the model matrix 
        // mModelMatrix = computeModelMatrix( mPosition, mRotationMatrix, mScale );
        computeModelMatrix();
    }

    // Set rotation
    void CollisionBody::setRotation( float angle, glm::vec3 axis )
    {
        // Compute the rotation matrix from the angle and axis given
        mRotationMatrix = glm::mat4( 1.f );
        if (angle != 0.)
            mRotationMatrix = glm::rotate( mRotationMatrix, glm::radians(angle), 
                                           glm::normalize(axis) );

        // Update the model matrix 
        // mModelMatrix = computeModelMatrix( mPosition, mRotationMatrix, mScale );
        computeModelMatrix();
    }

    // Method to compute a model matrix 
    // glm::mat4 CollisionBody::computeModelMatrix( const glm::vec3& translation, 
    //                                              const glm::mat4 rotationMatrix, 
    //                                              const glm::vec3& scale )
    void CollisionBody::computeModelMatrix()
    {
        mModelMatrix = glm::mat4( 1.f );
        mModelMatrix = glm::translate(mModelMatrix, mPosition);
        mModelMatrix = mModelMatrix * mRotationMatrix;
        mModelMatrix = glm::scale(mModelMatrix, mScale);
        // // return modelMatrix;

        // Compute the model matrix for the geometry object
        mGeometryObject->setModelMatrix( mPosition, mRotationMatrix, mScale );
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
        // mGeometryObject->setModelMatrix( mPosition, mRotationMatrix, mScale );
        //////////////////////////
        //////////////////////////
        shader.setMat4( "model", mGeometryObject->getModelMatrix() );
        // shader.setMat4( "model", mModelMatrix );

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
        // mAcceleration { glm::vec3( 0.f, 0.f, 0.f ) },
        mGravity { glm::vec3( 0.f, -9.8f, 0.f ) },
        mDamping { 0.995f },
        mForceAccum { glm::vec3( 0.f, 0.f, 0.f ) }
        // mTorqueAccum { glm::vec3( 0.f, 0.f, 0.f ) },
        // //
        // mAngularVelocity { glm::vec3( 0.f, 0.f, 0.f ) },
        // mForce { glm::vec3( 0.f, 0.f, 0.f ) }, 
        // mTorque { glm::vec3( 0.f, 0.f, 0.f ) }
    {
        // mGravity = glm::vec3( 0., 0., 0. );

    }

    // Set velocity and acceleration
    void RigidBody::setVelocity( glm::vec3 velocity )
    {
        mVelocity = velocity;
    }
    void RigidBody::setGravity( glm::vec3 gravity )
    {
        mGravity = gravity;
    }

    // Set velocity damping
    void RigidBody::setDamping( float damping )
    {
        mDamping = damping;
    }

    // Set mass 
    void RigidBody::setMass( float mass )
    {
        mMass = mass;
        mMassInver = 1.f / mass;
    }
    void RigidBody::setInvMass( float invMass )
    {
        mMassInver = invMass;
        if ( invMass == 0.f )
            mMass = -1.f;
        else
            mMass = 1.f / invMass;
    }

    // // Update movement
    // void RigidBody::updateMovement( float deltaTime )
    // {
    //     std::cout << "RigidBody::updateMovement deprecated" << std::endl;
    //     // // Update velocity, adding the acceleration from the force and the gravity
    //     // mVelocity += ( mForce * mMassInver + mGravity ) * deltaTime;
    //     // // Update angular velocity
    //     // mAngularVelocity += mTorque * mMassInver * deltaTime;
    //     //
    //     // // Update the position
    //     // mPosition += mVelocity * deltaTime;
    //     //
    //     // // Compute the rotation angle and axis
    //     // float angVelLength = glm::length( mAngularVelocity );
    //     // if ( fabs(angVelLength) > 0.001 )
    //     // {
    //     //     float rotationAngle = RAD_TO_DEG * angVelLength * deltaTime;
    //     //     // Update the rotation
    //     //     mRotationMatrix = glm::rotate( mRotationMatrix, rotationAngle, 
    //     //                                    mAngularVelocity / angVelLength );
    //     // }
    //     //
    //     // // Update the model matrix 
    //     // // computeModelMatrix( mPosition, mRotationMatrix, mScale );
    //     // computeModelMatrix();
    //     //
    //     // // Move the collider
    //     // mCollider->moveCollider( mModelMatrix );
    //     //
    //     // // Reset the net force and torque on the object
    //     // mForce = glm::vec3( 0.f, 0.f, 0.f );
    //     // mTorque = glm::vec3( 0.f, 0.f, 0.f );
    // }

    // Integrate forward in time by the given duration
    void RigidBody::integrate( float deltaTime )
    {
        // Compute acceleration from the force
        glm::vec3 resultingAcc = mGravity + mForceAccum * mMassInver;
        // Update linear velocity
        mVelocity += resultingAcc * deltaTime;
        // Drag on the velocity, so it does not increase due to numerical errors
        mVelocity *= powf( mDamping, deltaTime );

        // Update the position
        mPosition += mVelocity * deltaTime;

        // Update the model matrix 
        // computeModelMatrix( mPosition, mRotationMatrix, mScale );
        computeModelMatrix();

        // Move the collider
        mCollider->moveCollider( mModelMatrix );

        // Reset the net force and torque on the object
        mForceAccum = glm::vec3( 0.f, 0.f, 0.f );
        // mTorqueAccum = glm::vec3( 0.f, 0.f, 0.f );
    }
}
