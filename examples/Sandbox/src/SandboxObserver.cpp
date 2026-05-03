#include "SandboxObserver.h"
#include "src/logger.h"

CameraFollowEntityObserver::CameraFollowEntityObserver(GLBase::Camera *cameraPtr, const glm::vec3 &initialObjPos)
    : mCamera(cameraPtr), mLastObjectPos(initialObjPos) {
    // Set the parameters of camera tracking to be over cricital damping,
    //  damping = 2 * sqrt(stiffness)
    mCamera->setTrackingParameters(5.f, 5.f, 50.f, 20.f);

    // Place the camera so it looks and follows the object
    mCamera->setPosition(initialObjPos + glm::vec3(-5., 0., -4.));
    mCamera->lookAtPoint(initialObjPos);
}

void CameraFollowEntityObserver::onEntityUpdated(const Entity &entity, float deltaTime) {
    const glm::vec3 &currObjectPos = entity.getPosition();

    if (glm::length(currObjectPos - mLastObjectPos) > 0.1f) {
        // The second parameter is the object's influece in the camera's orientation.
        // It would make sense if it depends on the object's velocity.
        mCamera->setLookAtTarget(currObjectPos, 1.f);
    }
}
