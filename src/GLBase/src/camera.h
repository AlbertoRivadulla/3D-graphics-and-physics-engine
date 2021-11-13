#ifndef CAMERA_H
#define CAMERA_H

#include "GLBase.h"

namespace GLBase
{
    // Default camera values
    // Angle with respect to the camera's y axis.
    // Default to -90 degrees. Otherwise the camera will be looking to the right.
    const float YAW { -90. }; 
    // Angle with respect to the camera's x axis.
    const float PITCH { 0. }; 
    const float SPEED { 10. };
    const float SENSITIVITY { 0.1 };
    const float FOV { 45. }; // Field of view

    // Forward declare the camera class
    class Camera;

    // Input handlers for the camera
    class CameraKeyboardInputHandler : public KeyboardInputHandler
    {
        public:
            // Constructor
            CameraKeyboardInputHandler(Camera* camera);

            // Method to process input
            void process(GLFWwindow* window, float deltaTime) const;

        private:
            // Pointer to the camera
            Camera* mCamera;
    };

    class CameraMouseInputHandler : public MouseInputHandler
    {
        public:
            // Constructor
            CameraMouseInputHandler(Camera* camera);

            // Method to process input
            void process(double xpos, double ypos) const;

        private:
            // Pointer to the camera
            Camera* mCamera;
    };

    class CameraScrollInputHandler : public ScrollInputHandler
    {
        public:
            // Constructor
            CameraScrollInputHandler(Camera* camera);

            // Method to process input
            void process(double xoffset, double yoffset) const;

        private:
            // Pointer to the camera
            Camera* mCamera;
    };

    // Abstract camera class
    class Camera 
    {
        // Declare the input handler classes as friends
        friend class CameraKeyboardInputHandler;
        friend class CameraMouseInputHandler;
        friend class CameraScrollInputHandler;

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

            // Input handlers
            CameraKeyboardInputHandler mKeyboardHandler;
            CameraMouseInputHandler mMouseHandler;
            CameraScrollInputHandler mScrollHandler;

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
