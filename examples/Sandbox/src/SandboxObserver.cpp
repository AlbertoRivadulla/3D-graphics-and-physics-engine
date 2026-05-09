#include "SandboxObserver.h"

CameraFollowEntityObserver::CameraFollowEntityObserver(GLBase::Camera *cameraPtr, const glm::vec3 &initialObjPos)
    : mCamera(cameraPtr), mLastObjectPos(initialObjPos) {
    // Set the parameters of camera tracking to be over cricital damping,
    //  damping = 2 * sqrt(stiffness)
    mCamera->setTrackingParameters(1.f, 5.f, 10.f, 6.5f, 100.f, 20.f);

    mCameraPositionOffset = glm::vec3(-5., 3., 0.);

    // Place the camera so it looks and follows the object
    mCamera->setPosition(initialObjPos + mCameraPositionOffset);
    mCamera->setPositionTarget(initialObjPos + mCameraPositionOffset);
    mCamera->lookAtPoint(initialObjPos);
}

void CameraFollowEntityObserver::onEntityUpdated(const Entity &entity, float deltaTime) {
    const glm::vec3 &currObjectPos = entity.getPosition();

    if (glm::length(currObjectPos - mLastObjectPos) > 0.1f) {
        // The second parameter is the object's influence in the camera's orientation.
        // It would make sense if it depends on the object's velocity.
        mCamera->setLookAtTarget(currObjectPos, 1.f);

        mCamera->setPositionTarget(currObjectPos + mCameraPositionOffset);
    }
}
