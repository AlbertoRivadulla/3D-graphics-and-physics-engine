#ifndef PLANE_OBSERVER_H
#define PLANE_OBSERVER_H

#include "interfaces.h"
#include "Entity.h"
#include "camera.h"
#include "utils.h"

class CameraFollowPlaneObserver : public IEntityObserver {
public:
    CameraFollowPlaneObserver(GLBase::Camera *cameraPtr, const glm::vec3 &initialObjPos);

    void onEntityUpdated(const Entity &entity, float deltaTime) override;

private:
    GLBase::OrbitalCamera *mCamera;

    float mCameraDistance;
    float mCameraHeight;

    glm::vec3 mLastObjectPos;
};

#endif
