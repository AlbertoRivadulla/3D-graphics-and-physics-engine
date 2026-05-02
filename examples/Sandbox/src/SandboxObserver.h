#ifndef SANDBOX_OBSERVER_H
#define SANDBOX_OBSERVER_H

#include "interfaces.h"
#include "Entity.h"
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

#endif
