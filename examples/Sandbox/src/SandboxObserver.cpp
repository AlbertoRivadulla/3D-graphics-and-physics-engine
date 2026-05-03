#include "SandboxObserver.h"
#include "src/logger.h"

CameraFollowEntityObserver::CameraFollowEntityObserver(GLBase::Camera *cameraPtr) : mCamera(cameraPtr) {}

void CameraFollowEntityObserver::onEntityUpdated(const Entity &entity, float deltaTime) {
    mCamera->lookAtPoint(entity.getPosition());
}
