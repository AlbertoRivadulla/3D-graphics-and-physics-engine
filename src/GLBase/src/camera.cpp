#include "camera.h"
#include "utils.h"

namespace GLBase {
//==============================
// Methods of the base Camera class
//==============================

// Constructor with vector values
Camera::Camera(int width, int height, glm::vec3 position, glm::vec3 up, float yaw, float pitch)
    : mKeyboardHandler(this), mMouseHandler(this), mScrollHandler(this), mWidth{width}, mHeight{height}, mNear{0.1},
      mFar{100.}, mFov{FOV}, mPosition{position}, mWorldUp{up}, mYaw{yaw}, mPitch{pitch},
      mFront{glm::vec3(0., 0., -1.)}, mMovementSpeed{SPEED}, mMouseSensitivity{MOUSE_SENSITIVITY}, mLastX{0.},
      mLastY{0.}, mFirstMouse{true}, mOrthoHalfWidth{3.f * (float)width / (float)height}, mOrthoHalfHeight{3.f},
      mIsOrthographic{false} {
    updateCameraVectors();
}

// Constructor with scalar values
Camera::Camera(int width, int height, float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw,
               float pitch)
    : mKeyboardHandler(this), mMouseHandler(this), mScrollHandler(this), mWidth{width}, mHeight{height}, mNear{0.1},
      mFar{100.}, mFov{FOV}, mPosition{glm::vec3(posX, posY, posZ)}, mWorldUp{glm::vec3(upX, upY, upZ)}, mYaw{yaw},
      mPitch{pitch}, mFront{glm::vec3(0., 0., -1.)}, mMovementSpeed{SPEED}, mMouseSensitivity{MOUSE_SENSITIVITY},
      mLastX{0.}, mLastY{0.}, mFirstMouse{true}, mOrthoHalfWidth{3.f * (float)width / (float)height},
      mOrthoHalfHeight{3.f}, mIsOrthographic{false} {
    updateCameraVectors();
}

// Method to make the camera orthographic
void Camera::setOrthographic() { mIsOrthographic = true; }
// Method to make the camera perspective
void Camera::setPerspective() { mIsOrthographic = false; }

// Method to set the frustum of the camera
void Camera::setFrustum(float near, float far) {
    mNear = near;
    mFar = far;
}

// Method to change the width and height of the viewport
void Camera::setDimensions(int width, int height) {
    mWidth = width;
    mHeight = height;
}

// Method to set the dimension of the orthographic projection matrix
void Camera::setOrthographicSize(float size) {
    mOrthoHalfWidth = size * ((float)mWidth / (float)mHeight) / 2.f;
    mOrthoHalfHeight = size / 2.f;
}

void Camera::lookAtPoint(glm::vec3 target) {
    mFront = glm::normalize(target - mPosition);

    mYaw = atan2f(mFront.z, mFront.x);
    mPitch = glm::clamp(asinf(mFront.y), glm::radians(-89.f), glm::radians(89.f));

    computeRightUpVectors();
}

const glm::vec3 &Camera::getPosition() const { return mPosition; }

void Camera::setPosition(glm::vec3 position) { mPosition = position; }

// Method to get the projection matrix
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

// Method to get the near and far planes of the frustum
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

// Calculate the front vector from the camera's updated Euler angles
void Camera::updateCameraVectors() {
    // Calculare the front vector
    glm::vec3 front;
    front.x = cos(mYaw) * cos(mPitch);
    front.y = sin(mPitch);
    front.z = sin(mYaw) * cos(mPitch);
    mFront = glm::normalize(front);

    computeRightUpVectors();
}

void Camera::computeRightUpVectors() {
    mRight = glm::normalize(glm::cross(mFront, mWorldUp));
    mUp = glm::normalize(glm::cross(mRight, mFront));
}

// Method to obtain the two possible projections
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

//==============================
// Methods of the input handlers for the camera
//==============================

// Keyboard input handler
//==============================

// Constructor
CameraKeyboardInputHandler::CameraKeyboardInputHandler(Camera *camera) { mCamera = camera; }

// Method to process input
void CameraKeyboardInputHandler::processInput(GLFWwindow *window, float deltaTime) const {
    // The keys WASD move the camera around the scene
    float cameraSpeed{mCamera->mMovementSpeed * deltaTime};
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        cameraSpeed *= 5.f;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        mCamera->mPosition += cameraSpeed * mCamera->mFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        mCamera->mPosition -= cameraSpeed * mCamera->mFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        mCamera->mPosition -= cameraSpeed * mCamera->mRight;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        mCamera->mPosition += cameraSpeed * mCamera->mRight;
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        mCamera->mPosition += cameraSpeed * mCamera->mUp;
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        mCamera->mPosition -= cameraSpeed * mCamera->mUp;
}

// Mouse input handler
//==============================

// Constructor
CameraMouseInputHandler::CameraMouseInputHandler(Camera *camera) { mCamera = camera; }

// Method to process input
void CameraMouseInputHandler::processInput(double xpos, double ypos) const {
    // If it is the first time that the mouse is moved, the last position is
    // the same as the current one
    if (mCamera->mFirstMouse) {
        mCamera->mLastX = xpos;
        mCamera->mLastY = ypos;
        mCamera->mFirstMouse = false;
    }

    // Compute the change in the position of the mouse since the previous frame
    float xoffset = xpos - mCamera->mLastX;
    float yoffset = mCamera->mLastY - ypos; // reversed since y-coordinates go from bottom to top
    // Update the previous position of the mouse stored
    mCamera->mLastX = xpos;
    mCamera->mLastY = ypos;
    // Multiply the offsets by the mouse sensitivity value
    xoffset *= mCamera->mMouseSensitivity;
    yoffset *= mCamera->mMouseSensitivity;

    // Add the offset values to the global variables yaw and pitch (rotation
    // around the camera's vertical (y) and horizontal (x) axes)
    mCamera->mYaw += xoffset;
    mCamera->mPitch += yoffset;

    // Constrain the pitch to be between (-90, 90) degrees
    if (mCamera->mPitch > glm::radians(89.))
        mCamera->mPitch = glm::radians(89.);
    if (mCamera->mPitch < glm::radians(-89.))
        mCamera->mPitch = glm::radians(-89.);

    // Update the Front, Up and Right camera vectors
    mCamera->updateCameraVectors();
}

// Scroll input handler
//==============================

// Constructor
CameraScrollInputHandler::CameraScrollInputHandler(Camera *camera) { mCamera = camera; }

// Method to process input
void CameraScrollInputHandler::processInput(double xoffset, double yoffset) const {
    // Change the field of view with vertical scroll.
    mCamera->mFov -= (float)yoffset;
    // Constraint it to be between (1, 45) degrees
    if (mCamera->mFov < 1.)
        mCamera->mFov = 1.;
    if (mCamera->mFov > 45.)
        mCamera->mFov = 45.;
}

} // namespace GLBase
