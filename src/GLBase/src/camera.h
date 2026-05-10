#ifndef CAMERA_H
#define CAMERA_H

#include <vector>
#include <numbers>
#include <glm/glm.hpp>
#include "utils.h"

namespace GLBase {
// Default camera values
// Angle with respect to the camera's y axis.
// Default to -90 degrees. Otherwise the camera will be looking to the right.
constexpr float YAW{glm::radians(-90.)};
// Angle with respect to the camera's x axis.
constexpr float PITCH{glm::radians(0.)};
constexpr float ROLL{glm::radians(0.)};
constexpr float SPEED{10.};
// const float SENSITIVITY{0.1};
constexpr float MOUSE_SENSITIVITY{0.1 * std::numbers::pi / 180.};
constexpr float FOV_DEFAULT{glm::radians(45.)}; // Field of view
constexpr float FOV_MIN{glm::radians(1.f)};
constexpr float FOV_MAX{glm::radians(90.f)};

class Camera {
public:
    Camera() = default;

    // Constructor with vector values
    Camera(int width, int height, glm::vec3 position = glm::vec3(0., 0., 0.), glm::vec3 up = glm::vec3(0., 1., 0.),
           float yaw = YAW, float pitch = PITCH, float roll = ROLL);

    // Constructor with scalar values
    Camera(int width, int height, float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw = YAW,
           float pitch = PITCH, float roll = ROLL);

    virtual ~Camera() = default;

    void setOrthographic();
    void setPerspective();

    void setFrustum(float near, float far);
    void setViewportDimensions(int width, int height);
    // Method to set the dimension of the orthographic projection matrix
    void setOrthographicSize(float size);

    // Set parameters for the camera tracking an object
    void setTrackingParameters(float mouseDecayRate = 0.f, float objectTargetDecayRate = 0.f,
                               float orientationStiffness = 0.f, float orientationDamping = 0.f,
                               float positionStiffness = 0.f, float positionDamping = 0.f);

    // Look at a given direction in world space
    void lookAtDirection(glm::vec3 direction);

    // Look at a given point in world space
    void lookAtPoint(glm::vec3 target);

    // Set the target to look at
    void setLookAtTarget(glm::vec3 target, float objectTargetInfluence = 1.f);

    void setPositionTarget(glm::vec3 targetPosition);

    const glm::vec3 &getPosition() const;
    void setPosition(glm::vec3 position);

    void moveFrontwards(float distance);
    void moveRightwards(float distance);
    void moveUpwards(float distance);

    void moveMouseTargetAngles(float xDistance, float yDistance);

    void increaseDecreaseFov(float fovChange);

    virtual void update(float deltaTime);

    glm::mat4 getProjectionMatrix();

    // Compute the view matrix calculated from the Euler angles
    glm::mat4 getViewMatrix();

    void getNearFarPlanes(float &near, float &far) const;

    std::vector<glm::vec4> algo() const;

    // Get the position of the eight corners of the frustum
    std::vector<glm::vec4> getFrustumCornersWorldSpace() const;
    // Get the position of the eight corners of the subfrustum index of the
    // total of subfrustums
    std::vector<glm::vec4> getFrustumCornersWorldSpace(const float &zNear, const float &zFar) const;

protected:
    glm::vec3 mPosition;
    // Euler angles of the camera
    float mYaw;
    float mPitch;
    float mRoll;

    // Mouse driven orientation
    float mMouseInfluence; // 1 when the mouse just moved
    float mMouseDecayRate; // 0 = no decay of mouse angles
    float mMouseTargetYaw;
    float mMouseTargetPitch;

    // Target driven orientation
    float mObjectTargetInfluence; // 1 when the target just moved
    float mObjectTargetDecayRate; // 0 = no decay of target angles
    float mObjectTargetYaw;
    float mObjectTargetPitch;
    float mObjectTargetRoll;
    float mOrientationStiffness;
    float mOrientationDamping;
    float mYawVelocity;
    float mPitchVelocity;
    float mRollVelocity;

    // Target driven position
    float mPositionStiffness;
    float mPositionDamping;
    glm::vec3 mObjectTargetPosition;
    glm::vec3 mPositionVelocity;

    // This is a constant
    glm::vec3 mWorldUp;

    void updateInfluences(float deltaTime);

    // Calculate the front vector from the camera's updated Euler angles
    void updateCameraVectors();
    void computeRightUpVectors();

private:
    // Width and height of the viewport
    int mWidth;
    int mHeight;
    // Near and far planes of the frustum
    float mNear;
    float mFar;

    float mFov;

    // Camera attributes
    // These are computed from the ones above
    glm::vec3 mFront;
    glm::vec3 mUp;
    glm::vec3 mRight;

    // Dimensions of the orthographic camera
    float mOrthoHalfWidth;
    float mOrthoHalfHeight;

    // View and projection matrices
    glm::mat4 mViewMatrix;
    glm::mat4 mProjectionMatrix;

    // Bool that says if the camera is orthographic or perspective
    bool mIsOrthographic;

    // Move in a spring-like manner towards the target yaw and pitch
    void springTowardsTarget(float deltaTime);

    // Method to obtain the two possible projections
    glm::mat4 getPerspectiveProjection();
    glm::mat4 getOrthographicProjection();

    std::vector<glm::vec4> computeFrustumCornersWorldSpace(const glm::mat4 &projectionMatrix) const;
};

class OrbitalCamera : public Camera {
public:
    OrbitalCamera(int width, int height, glm::vec3 position = glm::vec3(0., 0., 0.), glm::vec3 up = glm::vec3(0., 1., 0.),
           float yaw = YAW, float pitch = PITCH);

    void setTargetDistance(float targetDistance, float verticalPositionOffset);

    void setOrbitTarget(glm::vec3 targetPosition, glm::vec3 targetForward, glm::vec3 targetUp, float objectTargetInfluence);

    void update(float deltaTime) override;
private:
    float mDistanceToObject;
    float mDistanceVelocity;
    float mTargetDistance;
    float mVerticalPositionOffset;

    // Move in a spring-like manner towards the target yaw and pitch
    float springTowardsTargetOrbital(float deltaTime);
};

} // namespace GLBase

#endif
