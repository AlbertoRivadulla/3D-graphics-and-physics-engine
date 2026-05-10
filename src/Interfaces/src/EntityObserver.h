#ifndef ENTITY_OBSERVER_H
#define ENTITY_OBSERVER_H

class IEntityObserverBase {
public:
    virtual ~IEntityObserverBase() = default;
};

template <typename T> class IEntityObserverT : public IEntityObserverBase {
public:
    virtual void onEntityUpdated(const T &entity, float deltaTime) = 0;
};

class Entity;

using IEntityObserver = IEntityObserverT<Entity>;

#endif
