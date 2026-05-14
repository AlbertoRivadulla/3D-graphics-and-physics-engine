#include "camera.h"
#include "glm/ext/quaternion_trigonometric.hpp"
#include "src/logger.h"
#include "utils.h"

namespace GLBase {
//==============================
// Methods of the base Camera class
//==============================

// Constructor with vector values
Camera::Camera(int width, int height, glm::vec3 position, glm::vec3 up, float yaw, float pitch, float roll)
    : mPosition{position}, mYaw{yaw}, mPitch{pitch}, mRoll{roll}, mYawVelocity{0.f}, mPitchVelocity{0.f},
      mRollVelocity{0.f}, mPositionVelocity(glm::vec3(0.f, 0.f, 0.f)), mWorldUpNormalized{glm::normalize(up)},
      mWidth{width}, mHeight{height}, mNear{0.1}, mFar{100.}, mFov{FOV_DEFAULT}, mFront{glm::vec3(0., 0., -1.)},
      mOrthoHalfWidth{3.f * (float)width / (float)height}, mOrthoHalfHeight{3.f}, mIsOrthographic{false} {
    mObjectTargetPosition = mPosition;

    setTrackingParameters();
    updateCameraVectors();
}

// Constructor with scalar values
Camera::Camera(int width, int height, float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw,
               float pitch, float roll)
    : Camera(width, height, glm::vec3(posX, posY, posZ), glm::vec3(upX, upY, upZ), yaw, pitch, roll) {}

void Camera::setOrthographic() { mIsOrthographic = true; }
void Camera::setPerspective() { mIsOrthographic = false; }

void Camera::setFrustum(float near, float far) {
    mNear = near;
    mFar = far;
}

void Camera::setViewportDimensions(int width, int height) {
    mWidth = width;
    mHeight = height;
}

// Method to set the dimension of the orthographic projection matrix
void Camera::setOrthographicSize(float size) {
    mOrthoHalfWidth = size * ((float)mWidth / (float)mHeight) / 2.f;
    mOrthoHalfHeight = size / 2.f;
}

// Set parameters for the camera tracking an object
void Camera::setTrackingParameters(float mouseDecayRate, float objectTargetDecayRate, float orientationStiffness,
                                   float orientationDamping, float positionStiffness, float positionDamping) {
    mMouseDecayRate = mouseDecayRate;
    mObjectTargetDecayRate = objectTargetDecayRate;

    mOrientationStiffness = orientationStiffness;
    mOrientationDamping = orientationDamping;

    mPositionStiffness = positionStiffness;
    mPositionDamping = positionDamping;
}

void Camera::lookAtDirection(glm::vec3 direction) {
    glm::vec3 dirNormalized = glm::normalize(direction);

    mYaw = atan2f(dirNormalized.z, dirNormalized.x);
    mPitch = glm::clamp(asinf(dirNormalized.y), glm::radians(-89.f), glm::radians(89.f));

    mObjectTargetYaw = mYaw;
    mObjectTargetPitch = mPitch;
    mObjectTargetInfluence = 0.f;

    updateCameraVectors();
}

void Camera::lookAtPoint(glm::vec3 point) { lookAtDirection(point - mPosition); }

void Camera::setLookAtTarget(glm::vec3 target, float objectTargetInfluence) {
    glm::vec3 targetDirection = glm::normalize(target - mPosition);

    mObjectTargetYaw = atan2f(targetDirection.z, targetDirection.x);
    mObjectTargetPitch = glm::clamp(asinf(targetDirection.y), glm::radians(-89.f), glm::radians(89.f));

    mObjectTargetInfluence = objectTargetInfluence;
}

void Camera::setPositionTarget(glm::vec3 targetPosition) { mObjectTargetPosition = targetPosition; }

const glm::vec3 &Camera::getPosition() const { return mPosition; }

void Camera::setPosition(glm::vec3 position) {
    mPosition = position;
    mObjectTargetPosition = position;
}

void Camera::moveFrontwards(float distance) { mPosition += distance * mFront; }

void Camera::moveRightwards(float distance) { mPosition += distance * mRight; }

void Camera::moveUpwards(float distance) { mPosition += distance * mUp; }

void Camera::moveMouseTargetAngles(float xDistance, float yDistance) {
    if (mMouseInfluence < 0.0001f) {
        // The mouse was idle, so sync the angles to the current values
        mMouseTargetYaw = mYaw;
        mMouseTargetPitch = mPitch;
    }

    // Add the offset values to the variables yaw and pitch (rotation
    // around the camera's vertical (y) and horizontal (x) axes)
    mMouseTargetYaw = Utils::wrapAngle(mMouseTargetYaw + xDistance);
    mMouseTargetPitch = glm::clamp(mMouseTargetPitch + yDistance, glm::radians(-89.0f), glm::radians(89.0f));
    mMouseInfluence = 1.f;
}

void Camera::increaseDecreaseFov(float fovChange) { mFov = glm::clamp(mFov + fovChange, FOV_MIN, FOV_MAX); }

void Camera::update(float deltaTime) {
    updateInfluences(deltaTime);

    if (mObjectTargetInfluence > 0.5f && mMouseInfluence / mObjectTargetInfluence < 0.5f) {
        // As mouse influence decays, pull mouse target toward entity target
        // so when mouse fully releases there is no gap between the two
        float syncRate = 1.0f - mMouseInfluence; // 0 when mouse active, 1 when idle
        mMouseTargetYaw = Utils::wrapAngle(mMouseTargetYaw +
                                           syncRate * Utils::wrapAngle(mObjectTargetYaw - mMouseTargetYaw) * deltaTime);
        mMouseTargetPitch += syncRate * (mObjectTargetPitch - mMouseTargetPitch) * deltaTime;
    }

    float totalInfluence = mMouseInfluence + mObjectTargetInfluence;
    if (totalInfluence < 0.001f) {
        // Nothing is moving
        return;
    }

    springTowardsTarget(deltaTime);

    // Mouse drives directly on top, no spring
    if (mMouseInfluence > 0.001f) {
        float mouseWeight = mMouseInfluence / totalInfluence;
        mYaw = Utils::wrapAngle(mYaw + Utils::wrapAngle(mMouseTargetYaw - mYaw) * mouseWeight);
        mPitch =
            glm::clamp(glm::mix(mPitch, mMouseTargetPitch, mouseWeight), glm::radians(-89.0f), glm::radians(89.0f));

        // Kill spring velocity so there's no jerk when mouse releases
        mYawVelocity = 0.0f;
        mPitchVelocity = 0.0f;
        mRollVelocity = 0.0f;
    }

    updateCameraVectors();
}

glm::mat4 Camera::getProjectionMatrix() {
    if (mIsOrthographic)
        mProjectionMatrix =
            glm::ortho(-mOrthoHalfWidth, mOrthoHalfWidth, -mOrthoHalfHeight, mOrthoHalfHeight, mNear, mFar);
    else
        mProjectionMatrix = glm::perspective(mFov, (float)mWidth / (float)mHeight, mNear, mFar);

    return mProjectionMatrix;
}

// Compute the view matrix calculated from the Euler angles
glm::mat4 Camera::getViewMatrix() {
    mViewMatrix = glm::lookAt(mPosition, mPosition + mFront, mUp);

    return mViewMatrix;
}

void Camera::getNearFarPlanes(float &near, float &far) const {
    near = mNear;
    far = mFar;
}

// Get the position of the eight corners of the frustum
std::vector<glm::vec4> Camera::getFrustumCornersWorldSpace() const {
    return computeFrustumCornersWorldSpace(mProjectionMatrix);
}

// Get the position of the eight corners of the subfrustum index of the
// total of subfrustums
std::vector<glm::vec4> Camera::getFrustumCornersWorldSpace(const float &zNear, const float &zFar) const {
    // // Compute the values of z at the near and far plane
    // zNear = mNear + (float)index * (mFar - mNear) / (float)total;
    // float zFar  { mNear + (float)(index + 1) * (mFar - mNear) / (float)total
    // };

    // Get the projection matrix of the subfrustum
    glm::mat4 subProjection{glm::perspective(mFov, (float)mWidth / (float)mHeight, zNear, zFar)};

    return computeFrustumCornersWorldSpace(subProjection);
}

void Camera::updateInfluences(float deltaTime) {
    mMouseInfluence *= expf(-mMouseDecayRate * deltaTime);
    mObjectTargetInfluence *= expf(-mObjectTargetDecayRate * deltaTime);
}

// Calculate the front vector from the camera's updated Euler angles
void Camera::updateCameraVectors() {
    glm::vec3 front;
    front.x = cos(mYaw) * cos(mPitch);
    front.y = sin(mPitch);
    front.z = sin(mYaw) * cos(mPitch);
    mFront = glm::normalize(front);

    computeRightUpVectors();
}

void Camera::computeRightUpVectors() {
    mUp = glm::angleAxis(mRoll, mFront) * mWorldUpNormalized;
    mRight = glm::normalize(glm::cross(mFront, mUp));

    // mRight = glm::normalize(glm::cross(mFront, mWorldUpNormalized));
    // mUp = glm::normalize(glm::cross(mRight, mFront));
}

// Move in a spring-like manner towards the target yaw and pitch
void Camera::springTowardsTarget(float deltaTime) {
    // Update orientations
    float yawAccel =
        Utils::wrapAngle(mObjectTargetYaw - mYaw) * mOrientationStiffness - mYawVelocity * mOrientationDamping;
    float pitchAccel = (mObjectTargetPitch - mPitch) * mOrientationStiffness - mPitchVelocity * mOrientationDamping;
    float rollAccel =
        Utils::wrapAngle(mObjectTargetRoll - mRoll) * mOrientationStiffness - mRollVelocity * mOrientationDamping;

    mYawVelocity += yawAccel * deltaTime;
    mPitchVelocity += pitchAccel * deltaTime;
    mRollVelocity += rollAccel * deltaTime;

    mYaw = Utils::wrapAngle(mYaw + mObjectTargetInfluence * mYawVelocity * deltaTime);
    mPitch = glm::clamp(mPitch + mObjectTargetInfluence * mPitchVelocity * deltaTime, glm::radians(-89.0f),
                        glm::radians(89.0f));
    mRoll = Utils::wrapAngle(mRoll + mObjectTargetInfluence * mRollVelocity * deltaTime);

    // Update the position
    glm::vec3 posAcceleration =
        (mObjectTargetPosition - mPosition) * mPositionStiffness - mPositionVelocity * mPositionDamping;
    mPositionVelocity += posAcceleration * deltaTime;
    mPosition += mPositionVelocity * deltaTime;
}

glm::mat4 Camera::getPerspectiveProjection() {
    return glm::perspective(mFov, (float)mWidth / (float)mHeight, mNear, mFar);
}

glm::mat4 Camera::getOrthographicProjection() {
    // float aspectRatio = (float)mWidth / (float)mHeight;
    // return glm::ortho(-5.0f * aspectRatio, 5.0f * aspectRatio, -5.0f, 5.0f,
    // mNear - 1.f, mFar);

    return glm::ortho(-mOrthoHalfWidth, mOrthoHalfWidth, -mOrthoHalfHeight, mOrthoHalfHeight, mNear, mFar);
}

// Get the position of the eight corners of the frustum
// Following https://learnopengl.com/Guest-Articles/2021/CSM
std::vector<glm::vec4> Camera::computeFrustumCornersWorldSpace(const glm::mat4 &projectionMatrix) const {
    // Compute the matrix to go from screen space coordinates to world
    // coordinates
    const glm::mat4 screenToWorld{glm::inverse(projectionMatrix * mViewMatrix)};

    // Initialize a vector for the eight corners of the frustum
    std::vector<glm::vec4> frustumCorners;
    frustumCorners.reserve(8);

    // Compute the eight coners in world space
    for (unsigned int x = 0; x < 2; ++x) {
        for (unsigned int y = 0; y < 2; ++y) {
            for (unsigned int z = 0; z < 2; ++z) {
                const glm::vec4 point = screenToWorld * glm::vec4(2.f * x - 1.f, 2.f * y - 1.f,
                                                                  // (2.f * z - 1.f) / 2.f,
                                                                  2.f * z - 1.f, 1.f);
                frustumCorners.push_back(point / point.w);
            }
        }
    }

    return frustumCorners;
}

OrbitalCamera::OrbitalCamera(int width, int height, glm::vec3 position, glm::vec3 up, float yaw, float pitch)
    : Camera(width, height, position, up, yaw, pitch), mDistanceToObject(0), mDistanceVelocity(0) {}

void OrbitalCamera::setTargetDistance(float targetDistance, float verticalPositionOffset) {
    mTargetDistance = targetDistance;
    mVerticalPositionOffset = verticalPositionOffset;
}

void OrbitalCamera::setOrbitTarget(glm::vec3 targetPosition, glm::vec3 targetForward, glm::vec3 targetUp,
                                   float objectTargetInfluence) {
    glm::vec3 forward = glm::normalize(targetForward);

    mObjectTargetYaw = atan2f(forward.z, forward.x);
    mObjectTargetPitch = glm::clamp(asinf(forward.y), -89.f, 89.f);
    mObjectTargetInfluence = objectTargetInfluence;

    // Compute target roll from the given up vector
    if (abs(glm::dot(forward, mWorldUpNormalized)) > 0.999f) {
        mObjectTargetRoll = 0.f;
    } else {
        glm::vec3 right = glm::normalize(glm::cross(forward, mWorldUpNormalized));
        mObjectTargetRoll = atan2f(glm::dot(targetUp, right), glm::dot(targetUp, mWorldUpNormalized));
    }

    glm::vec3 displTargetPosition = targetPosition + mVerticalPositionOffset * mWorldUpNormalized;
    mDistanceToObject = glm::length(displTargetPosition - mPosition);

    setPositionTarget(displTargetPosition);
}

void OrbitalCamera::update(float deltaTime) {
    updateInfluences(deltaTime);

    float totalInfluence = mMouseInfluence + mObjectTargetInfluence;
    if (totalInfluence < 0.001f)
        return;

    // Sync mouse target toward entity target as mouse influence decays
    if (mObjectTargetInfluence > 0.5f && mMouseInfluence / mObjectTargetInfluence < 0.5f) {
        float syncRate = 1.0f - mMouseInfluence;
        mMouseTargetYaw = Utils::wrapAngle(mMouseTargetYaw +
                                           syncRate * Utils::wrapAngle(mObjectTargetYaw - mMouseTargetYaw) * deltaTime);
        mMouseTargetPitch += syncRate * (mObjectTargetPitch - mMouseTargetPitch) * deltaTime;
    }

    // 1. Spring toward entity target only
    float resultDistance = springTowardsTargetOrbital(deltaTime);

    // 2. Mouse drives directly on top, no spring
    if (mMouseInfluence > 0.001f) {
        float mouseWeight = mMouseInfluence / totalInfluence;
        mYaw = Utils::wrapAngle(mYaw + Utils::wrapAngle(mMouseTargetYaw - mYaw) * mouseWeight);
        mPitch =
            glm::clamp(glm::mix(mPitch, mMouseTargetPitch, mouseWeight), glm::radians(-89.0f), glm::radians(89.0f));

        // Kill spring velocity so there's no jerk when mouse releases
        mYawVelocity = 0.0f;
        mPitchVelocity = 0.0f;
        mRollVelocity = 0.0f;
    }

    mPosition = mObjectTargetPosition - Utils::sphericalToCartesian(mYaw, mPitch, resultDistance);

    updateCameraVectors();
}

// Move in a spring-like manner towards the target yaw and pitch
float OrbitalCamera::springTowardsTargetOrbital(float deltaTime) {
    // Angular spring
    if (mObjectTargetInfluence > 0.001f) {
        float yawAccel =
            Utils::wrapAngle(mObjectTargetYaw - mYaw) * mOrientationStiffness - mYawVelocity * mOrientationDamping;
        float pitchAccel = (mObjectTargetPitch - mPitch) * mOrientationStiffness - mPitchVelocity * mOrientationDamping;
        float rollAccel =
            Utils::wrapAngle(mObjectTargetRoll - mRoll) * mOrientationStiffness - mRollVelocity * mOrientationDamping;

        mYawVelocity += yawAccel * deltaTime;
        mPitchVelocity += pitchAccel * deltaTime;
        mRollVelocity += rollAccel * deltaTime;

        mYaw = Utils::wrapAngle(mYaw + mObjectTargetInfluence * mYawVelocity * deltaTime);
        mPitch = glm::clamp(mPitch + mObjectTargetInfluence * mPitchVelocity * deltaTime, glm::radians(-89.0f),
                            glm::radians(89.0f));
        mRoll = Utils::wrapAngle(mRoll + mObjectTargetInfluence * mRollVelocity * deltaTime);
    }

    // Distance spring
    float distAccel = (mTargetDistance - mDistanceToObject) * mPositionStiffness - mDistanceVelocity * mPositionDamping;
    mDistanceVelocity += distAccel * deltaTime;

    return glm::clamp(mDistanceToObject + mDistanceVelocity * deltaTime, mTargetDistance, 2 * mTargetDistance);
}

} // namespace GLBase
