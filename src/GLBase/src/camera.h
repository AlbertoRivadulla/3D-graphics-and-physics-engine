#ifndef CAMERA_H
#define CAMERA_H

#include "GLBase.h"

namespace GLBase
{
    // // Define several possible options for the movement of the camera.
    // enum CameraMovement 
    // {
    //     FORWARD,
    //     BACKWARD,
    //     LEFT,
    //     RIGHT
    // };

    // Default camera values
    // Angle with respect to the camera's y axis.
    // Default to -90 degrees. Otherwise the camera will be looking to the right.
    const float YAW { -90. }; 
    // Angle with respect to the camera's x axis.
    const float PITCH { 0. }; 
    const float SPEED { 10. };
    const float SENSITIVITY { 0.1 };
    const float FOV { 45. }; // Field of view

    // Abstract camera class
    class Camera 
    {
        public:
            // Width and height of the viewport
            int mWidth;
            int mHeight;
            // Near and far planes of the frustum
            float mNear;
            float mFar;
            // Camera attributes
            glm::vec3 Position;
            glm::vec3 Front;
            glm::vec3 Up;
            glm::vec3 Right;
            glm::vec3 WorldUp;
            // Euler angles of the camera
            float Yaw;
            float Pitch;
            // Field of view
            float Fov;

            // Constructor with vector values
            Camera( int width, int height,
                    glm::vec3 position = glm::vec3(0., 0., 0.), 
                    glm::vec3 up = glm::vec3(0., 1., 0.),
                    float yaw = YAW, float pitch = PITCH);

            // Constructor with scalar values
            Camera( int width, int height,
                    float posX, float posY, float posZ,
                    float upX, float upY, float upZ,
                    float yaw = YAW, float pitch = PITCH);

            // Methods to configure the camera
            // Method to set the frustum of the camera
            void setFrustum(float near, float far);
            // Method to change the width and height of the viewport
            void setDimensions(int width, int height);
            
            // Method to obtain the two possible projections
            glm::mat4 getPerspectiveProjection();
            glm::mat4 getOrthographicProjection();

            // Compute the view matrix calculated from the Euler angles
            glm::mat4 getViewMatrix();

            // Function to process the keyboard input
            // void processKeyboardInput(CameraMovement direction, float deltaTime);
            void processKeyboardInput(GLFWwindow* window, float deltaTime);

            // Function to process the mouse input
            // void processMouseInput(float xoffset, float yoffset, bool constrainPitch = true);
            void processMouseInput(float xpos, float ypos);

            // Function to process the scroll
            void processScrollInput(float xoffset, float yoffset);

        private:
            // Camera options
            float MovementSpeed;
            float MouseSensitivity;

            // Last position of the mouse
            float mLastX;
            float mLastY;
            // Boolean variable that determines whether it is the first mouse mouvement in the execution
            bool mFirstMouse;

            // Calculate the front vector from the camera's updated Euler angles
            void updateCameraVectors();
    };

    class OrthographicCamera : public Camera
    {
        public:
            glm::mat4 getProjectionMatrix();
    };

    class PerspectiveCamera : public Camera
    {
        public:
            glm::mat4 getProjectionMatrix();
    };
}

#endif
