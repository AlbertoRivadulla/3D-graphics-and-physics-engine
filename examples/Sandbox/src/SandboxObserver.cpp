#include "SandboxObserver.h"
#include <memory>

CameraFollowEntityObserver::CameraFollowEntityObserver(GLBase::Camera *cameraPtr, const glm::vec3 &initialObjPos)
    : mCamera(dynamic_cast<GLBase::OrbitalCamera*>(cameraPtr)), mLastObjectPos(initialObjPos) {
    // Set the parameters of camera tracking to be over cricital damping,
    //  damping = 2 * sqrt(stiffness)
    mCamera->setTrackingParameters(10.f, 1.f, 50.f, 15.f, 100.f, 20.f);

    mCameraDistance = 5.f;
    mCameraHeight = 1.f;

    glm::vec3 lookAtDirection = glm::vec3(0., 0., 1.);
    glm::vec3 upDirection = glm::vec3(0., 1., 0.);

    mCamera->setTargetDistance(mCameraDistance, mCameraHeight);
    mCamera->setOrbitTarget(initialObjPos, lookAtDirection, upDirection, 1.f);

    // Place the camera so it looks and follows the object
    mCamera->setPosition(initialObjPos + mCameraHeight * upDirection - lookAtDirection * mCameraDistance);
    mCamera->lookAtDirection(lookAtDirection);
}

void CameraFollowEntityObserver::onEntityUpdated(const Entity &entity, float deltaTime) {
    const glm::vec3 &currObjectPos = entity.getPosition();

    if (glm::length(currObjectPos - mLastObjectPos) > 0.1f) {
        // The second parameter is the object's influence in the camera's orientation.
        // It would make sense if it depends on the object's velocity.
        glm::vec3 lookAtDirection = entity.getOrientation() * glm::vec3(0., 0., 1.);
        // glm::vec3 lookAtDirection = entity.getRigidBody()->getVelocity();
        glm::vec3 upDirection = entity.getOrientation() * glm::vec3(0., 1., 0.);
        mCamera->setOrbitTarget(currObjectPos, lookAtDirection, upDirection, 1.f);
    }
}
