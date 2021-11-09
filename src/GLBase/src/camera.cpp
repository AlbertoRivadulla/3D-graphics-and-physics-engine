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
          mWidth { width }, mHeight { height }, mNear { 0.1 }, mFar { 100. }
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
          mWidth { width }, mHeight { height }, mNear { 0.1 }, mFar { 100. }
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

    // Method to obtain the two possible projections
    glm::mat4 Camera::getPerspectiveProjection()
    {
        return glm::perspective(glm::radians(Fov), (float)mWidth / (float)mHeight, mNear, mFar);
    }

    glm::mat4 Camera::getOrthographicProjection()
    {
        return glm::ortho(0.f, (float)mWidth, (float)mHeight, 0.f, mNear, mFar);
    }

    // Compute the view matrix calculated from the Euler angles
    glm::mat4 Camera::getViewMatrix()
    {
        return glm::lookAt(Position, Position + Front, Up);
    }

    // Function to process the keyboard input
    // void Camera::processKeyboardInput(CameraMovement direction, float deltaTime)
    void Camera::processKeyboardInput(GLFWwindow* window, float deltaTime)
    {
        // The keys WASD move the camera around the scene
        float cameraSpeed { MovementSpeed * deltaTime };
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            Position += cameraSpeed * Front;
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            Position -= cameraSpeed * Front;
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            Position -= cameraSpeed * Right;
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            Position += cameraSpeed * Right;
    }

    // Function to process the mouse input
    // void Camera::processMouseInput(float xoffset, float yoffset, bool constrainPitch)
    void Camera::processMouseInput(float xpos, float ypos)
    {
        // If it is the first time that the mouse is moved, the last position is
        // the same as the current one
        if (mFirstMouse)
        {
            mLastX = xpos;
            mLastY = ypos;
            mFirstMouse = false;
        }

        // Compute the change in the position of the mouse since the previous frame
        float xoffset = xpos - mLastX;
        float yoffset = mLastY - ypos; // reversed since y-coordinates go from bottom to top
        // Update the previous position of the mouse stored
        mLastX = xpos;
        mLastY = ypos;
        // Multiply the offsets by the mouse sensitivity value
        xoffset *= MouseSensitivity;
        yoffset *= MouseSensitivity;

        // Add the offset values to the global variables yaw and pitch (rotation around
        // the camera's vertical (y) and horizontal (x) axes)
        Yaw += xoffset;
        Pitch += yoffset;

        // Constrain the pitch to be between (-90, 90) degrees
        if (Pitch > 89.)
            Pitch = 89.;
        if (Pitch < -89)
            Pitch = -89;

        // Update the Front, Up and Right camera vectors
        updateCameraVectors();
    }

    // Function to process the scroll
    void Camera::processScrollInput(float xoffset, float yoffset)
    {
        // Change the field of view with vertical scroll.
        Fov -= (float)yoffset;
        // Constraint it to be between (1, 45) degrees
        if (Fov < 1.)
            Fov = 1.;
        if (Fov > 45.)
            Fov = 45.;
    }

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

}
