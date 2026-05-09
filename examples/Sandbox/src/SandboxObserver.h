#ifndef SANDBOX_OBSERVER_H
#define SANDBOX_OBSERVER_H

#include "interfaces.h"
#include "Entity.h"
#include "camera.h"
#include "utils.h"

class PrintPositionEntityObserver : public IEntityObserver {
public:
    PrintPositionEntityObserver(std::string objectName) { mObjectName = objectName; }

    void onEntityUpdated(const Entity &entity, float deltaTime) override {
        LOG_INFO("[" << mObjectName << "] position: " << printVector(entity.getPosition()));
    }

private:
    std::string mObjectName;
};

class CameraFollowEntityObserver : public IEntityObserver {
public:
    CameraFollowEntityObserver(GLBase::Camera *cameraPtr, const glm::vec3 &initialObjPos);

    void onEntityUpdated(const Entity &entity, float deltaTime) override;

private:
    GLBase::OrbitalCamera *mCamera;

    float mCameraDistance;
    float mCameraHeight;

    glm::vec3 mLastObjectPos;
};

#endif
