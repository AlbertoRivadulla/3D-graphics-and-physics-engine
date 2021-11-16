#include "GLBase.h"

namespace GLBase
{
    //==============================
    // Methods of the base Camera class
    //==============================

    // Constructor with vector values
    Camera::Camera( int width, int height, glm::vec3 position, glm::vec3 up, 
                    float yaw, float pitch) 
        : Front { glm::vec3(0., 0., -1.) }, MovementSpeed { SPEED },
          MouseSensitivity { SENSITIVITY }, Fov { FOV },  
          Position { position }, WorldUp { up }, Yaw { yaw }, Pitch { pitch },
          mLastX { 0. }, mLastY { 0. }, mFirstMouse { true },
          mWidth { width }, mHeight { height }, mNear { 0.1 }, mFar { 100. },
          mOrthoHalfWidth { 3.f * (float)width / (float)height }, mOrthoHalfHeight { 3.f },
          mKeyboardHandler(this), mMouseHandler(this), mScrollHandler(this)
    {
        updateCameraVectors();
    }

    // Constructor with scalar values
    Camera::Camera( int width, int height, float posX, float posY, float posZ, 
                    float upX, float upY, float upZ, float yaw, float pitch) 
        : Front { glm::vec3(0., 0., -1.) }, MovementSpeed { SPEED },
          MouseSensitivity { SENSITIVITY }, Fov { FOV },  
          Position { glm::vec3(posX, posY, posZ) }, WorldUp { glm::vec3(upX, upY, upZ)},
          Yaw { yaw }, Pitch { pitch }, mLastX { 0. }, mLastY { 0. }, mFirstMouse { true },
          mWidth { width }, mHeight { height }, mNear { 0.1f }, mFar { 100.f },
          mOrthoHalfWidth { 3.f * (float)width / (float)height }, mOrthoHalfHeight { 3.f },
          mKeyboardHandler(this), mMouseHandler(this), mScrollHandler(this)
    {
        updateCameraVectors();
    }

    // Method to set the frustum of the camera
    void Camera::setFrustum(float near, float far)
    {
        mNear = near;
        mFar = far;
    }

    // Method to change the width and height of the viewport
    void Camera::setDimensions(int width, int height)
    {
        mWidth = width;
        mHeight = height;
    }

    // Method to set the dimension of the orthographic projection matrix
    void Camera::setOrthographicSize(float size)
    {
        mOrthoHalfWidth = size * ( (float)mWidth / (float)mHeight ) / 2.f;
        mOrthoHalfHeight = size / 2.f;
    }

    // Method to obtain the two possible projections
    glm::mat4 Camera::getPerspectiveProjection()
    {
        return glm::perspective(glm::radians(Fov), (float)mWidth / (float)mHeight, mNear, mFar);
    }

    glm::mat4 Camera::getOrthographicProjection()
    {
        // float aspectRatio = (float)mWidth / (float)mHeight;
        // return glm::ortho(-5.0f * aspectRatio, 5.0f * aspectRatio, -5.0f, 5.0f, mNear - 1.f, mFar);

        return glm::ortho(-mOrthoHalfWidth, mOrthoHalfWidth, -mOrthoHalfHeight, mOrthoHalfHeight, mNear, mFar);
    }

    // Compute the view matrix calculated from the Euler angles
    glm::mat4 Camera::getViewMatrix()
    {
        return glm::lookAt(Position, Position + Front, Up);
    }

    // // Function to process the keyboard input
    // // void Camera::processKeyboardInput(CameraMovement direction, float deltaTime)
    // void Camera::processKeyboardInput(GLFWwindow* window, float deltaTime)
    // {
    //     // The keys WASD move the camera around the scene
    //     float cameraSpeed { MovementSpeed * deltaTime };
    //     if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    //         Position += cameraSpeed * Front;
    //     if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    //         Position -= cameraSpeed * Front;
    //     if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    //         Position -= cameraSpeed * Right;
    //     if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    //         Position += cameraSpeed * Right;
    // }

    // // Function to process the mouse input
    // // void Camera::processMouseInput(float xoffset, float yoffset, bool constrainPitch)
    // void Camera::processMouseInput(float xpos, float ypos)
    // {
    //     // If it is the first time that the mouse is moved, the last position is
    //     // the same as the current one
    //     if (mFirstMouse)
    //     {
    //         mLastX = xpos;
    //         mLastY = ypos;
    //         mFirstMouse = false;
    //     }
    //
    //     // Compute the change in the position of the mouse since the previous frame
    //     float xoffset = xpos - mLastX;
    //     float yoffset = mLastY - ypos; // reversed since y-coordinates go from bottom to top
    //     // Update the previous position of the mouse stored
    //     mLastX = xpos;
    //     mLastY = ypos;
    //     // Multiply the offsets by the mouse sensitivity value
    //     xoffset *= MouseSensitivity;
    //     yoffset *= MouseSensitivity;
    //
    //     // Add the offset values to the global variables yaw and pitch (rotation around
    //     // the camera's vertical (y) and horizontal (x) axes)
    //     Yaw += xoffset;
    //     Pitch += yoffset;
    //
    //     // Constrain the pitch to be between (-90, 90) degrees
    //     if (Pitch > 89.)
    //         Pitch = 89.;
    //     if (Pitch < -89)
    //         Pitch = -89;
    //
    //     // Update the Front, Up and Right camera vectors
    //     updateCameraVectors();
    // }

    // // Function to process the scroll
    // void Camera::processScrollInput(float xoffset, float yoffset)
    // {
    //     // Change the field of view with vertical scroll.
    //     Fov -= (float)yoffset;
    //     // Constraint it to be between (1, 45) degrees
    //     if (Fov < 1.)
    //         Fov = 1.;
    //     if (Fov > 45.)
    //         Fov = 45.;
    // }

    // Calculate the front vector from the camera's updated Euler angles
    void Camera::updateCameraVectors()
    {
        // Calculare the front vector
        glm::vec3 front;
        front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        front.y = sin(glm::radians(Pitch));
        front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        Front = glm::normalize(front);
        // Calculate the Right and Up vectors
        Right = glm::normalize(glm::cross(Front, WorldUp));
        Up = glm::normalize(glm::cross(Right, Front));
    }

    //==============================
    // Methods of the input handlers for the camera
    //==============================

    // Keyboard input handler
    //==============================

    // Constructor
    CameraKeyboardInputHandler::CameraKeyboardInputHandler(Camera* camera)
    {
        mCamera = camera;
    }

    // Method to process input
    void CameraKeyboardInputHandler::process(GLFWwindow* window, float deltaTime) const
    {
        // The keys WASD move the camera around the scene
        float cameraSpeed { mCamera->MovementSpeed * deltaTime };
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            mCamera->Position += cameraSpeed * mCamera->Front;
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            mCamera->Position -= cameraSpeed * mCamera->Front;
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            mCamera->Position -= cameraSpeed * mCamera->Right;
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            mCamera->Position += cameraSpeed * mCamera->Right;
    }
    
    // Mouse input handler
    //==============================

    // Constructor
    CameraMouseInputHandler::CameraMouseInputHandler(Camera* camera)
    {
        mCamera = camera;
    }

    // Method to process input
    void CameraMouseInputHandler::process(double xpos, double ypos) const
    {
        // If it is the first time that the mouse is moved, the last position is
        // the same as the current one
        if (mCamera->mFirstMouse)
        {
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
        xoffset *= mCamera->MouseSensitivity;
        yoffset *= mCamera->MouseSensitivity;

        // Add the offset values to the global variables yaw and pitch (rotation around
        // the camera's vertical (y) and horizontal (x) axes)
        mCamera->Yaw += xoffset;
        mCamera->Pitch += yoffset;

        // Constrain the pitch to be between (-90, 90) degrees
        if (mCamera->Pitch > 89.)
            mCamera->Pitch = 89.;
        if (mCamera->Pitch < -89)
            mCamera->Pitch = -89;

        // Update the Front, Up and Right camera vectors
        mCamera->updateCameraVectors();
    }

    // Scroll input handler
    //==============================

    // Constructor
    CameraScrollInputHandler::CameraScrollInputHandler(Camera* camera)
    {
        mCamera = camera;
    }

    // Method to process input
    void CameraScrollInputHandler::process(double xoffset, double yoffset) const
    {
        // Change the field of view with vertical scroll.
        mCamera->Fov -= (float)yoffset;
        // Constraint it to be between (1, 45) degrees
        if (mCamera->Fov < 1.)
            mCamera->Fov = 1.;
        if (mCamera->Fov > 45.)
            mCamera->Fov = 45.;
    }

}
