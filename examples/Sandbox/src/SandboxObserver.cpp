#include "SandboxObserver.h"
#include <memory>

CameraFollowEntityObserver::CameraFollowEntityObserver(GLBase::Camera *cameraPtr, const glm::vec3 &initialObjPos)
    : mCamera(dynamic_cast<GLBase::OrbitalCamera*>(cameraPtr)), mLastObjectPos(initialObjPos) {
    // Set the parameters of camera tracking to be over cricital damping,
    //  damping = 2 * sqrt(stiffness)
    mCamera->setTrackingParameters(1.f, 1.f, 10.f, 6.5f, 100.f, 20.f);

    mCameraDistance = 5.f;
    mCameraHeight = 1.f;

    // Place the camera so it looks and follows the object
    mCamera->setPosition(initialObjPos + glm::vec3(mCameraDistance, mCameraHeight, 0.f));
    mCamera->lookAtDirection(glm::vec3(1., 0., 0.));

    mCamera->setTargetDistance(mCameraDistance, mCameraHeight);

    glm::vec3 lookAtDirection = glm::vec3(1., 0., 0.);
    mCamera->setOrbitTarget(initialObjPos, lookAtDirection, 1.f);
}

void CameraFollowEntityObserver::onEntityUpdated(const Entity &entity, float deltaTime) {
    const glm::vec3 &currObjectPos = entity.getPosition();

    if (glm::length(currObjectPos - mLastObjectPos) > 0.1f) {
        // The second parameter is the object's influence in the camera's orientation.
        // It would make sense if it depends on the object's velocity.
        glm::vec3 lookAtDirection = entity.getOrientation() * glm::vec3(1., 0., 0.);
        mCamera->setOrbitTarget(currObjectPos, lookAtDirection, 1.f);
    }
}
