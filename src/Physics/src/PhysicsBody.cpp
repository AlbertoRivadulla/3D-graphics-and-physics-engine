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

    // Add a geometrical object that will not be drawn
    void CollisionBody::addGeometryNotDrawn( GLElemObject* objectPtr )
    {
        // Store the pointer to the geometry as a member object
        mGeometryObject = objectPtr;

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

    // Getters
    glm::vec3 CollisionBody::getPosition()
    {
        return mPosition;
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
    // void CollisionBody::draw( Shader& shader )
    void CollisionBody::draw()
    {
        // Configure the shader
        mMaterial->configShader( mGeometryObject->getModelMatrix() );

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
        mVelocity { velocity }, mMassInver { 1.f / mass },
        // mAcceleration { glm::vec3( 0.f, 0.f, 0.f ) },
        mDamping { 0.995f },
        mForceAccum { glm::vec3( 0.f, 0.f, 0.f ) }
        // mTorqueAccum { glm::vec3( 0.f, 0.f, 0.f ) },
        // //
        // mAngularVelocity { glm::vec3( 0.f, 0.f, 0.f ) },
        // mForce { glm::vec3( 0.f, 0.f, 0.f ) }, 
        // mTorque { glm::vec3( 0.f, 0.f, 0.f ) }
    {
        setMass( mass );

    }

    // Set velocity and acceleration
    void RigidBody::setVelocity( glm::vec3 velocity )
    {
        mVelocity = velocity;
    }

    // Set velocity damping
    void RigidBody::setDamping( float damping )
    {
        mDamping = damping;
    }

    // Set mass 
    void RigidBody::setMass( float mass )
    {
        if ( mass < 0.f )
        {
            mMass = -1.f;
            mMassInver = 0.f;
        }
        else
        {
            mMass = mass;
            mMassInver = 1.f / mass;
        }
    }
    void RigidBody::setInvMass( float invMass )
    {
        mMassInver = invMass;
        if ( invMass == 0.f )
            mMass = -1.f;
        else
            mMass = 1.f / invMass;
    }

    // Getters
    float RigidBody::getMass()
    {
        return mMass;
    }
    glm::vec3 RigidBody::getVelocity()
    {
        return mVelocity;
    }

    // Check if it has infinite mass
    bool RigidBody::hasInfiniteMass()
    {
        return mMassInver < 0.f;
    }

    // Add a force
    void RigidBody::addForce( const glm::vec3& force )
    {
        mForceAccum += force;
    }

    // Set the accumulators to zero
    void RigidBody::clearAccumulators()
    {
        mForceAccum = glm::vec3( 0.f, 0.f, 0.f );
        // mTorqueAccum = glm::vec3( 0.f, 0.f, 0.f );
    }

    // Integrate forward in time by the given duration
    void RigidBody::integrate( float deltaTime )
    {
        // Compute acceleration from the force
        glm::vec3 resultingAcc = mForceAccum * mMassInver;
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
        clearAccumulators();
    }
}
