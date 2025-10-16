#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "LibGL.h"

// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    UP,      // New movement for editor camera
    DOWN     // New movement for editor camera
};

// Default camera values
const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 2.5f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.0f;

// An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
class CameraCPP
{
public:
    // camera Attributes
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;
    // euler Angles
    float Yaw;
    float Pitch;
    // camera options
    float MovementSpeed;
    float MouseSensitivity;
    float Zoom;
    float ZoomMin;
    float ZoomMax;
    int Type;
    bool scrollEnabled;

    // constructor with vectors
    void InitCamera(
        glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
        float yaw = YAW,
        float pitch = PITCH,
        float zoom = ZOOM,
        float movementSpeed = SPEED,
        float mouseSensitivity = SENSITIVITY,
        int type = 0, bool enableZoomWheelMove = false
    )
    {
        Front = glm::vec3(0.0f, 0.0f, -1.0f);
        MovementSpeed = movementSpeed;
        MouseSensitivity = mouseSensitivity;
        Zoom = zoom;
        Position = position;
        WorldUp = up;
        Yaw = yaw;
        Pitch = pitch;
        Type = type;
        scrollEnabled = enableZoomWheelMove;

        ZoomMin = 25.0f;
        ZoomMax = 105.0f;
        updateCameraVectors();
    }


    // returns the view matrix calculated using Euler Angles and the LookAt Matrix
    glm::mat4 GetViewMatrix()
    {
        return glm::lookAt(Position, Position + Front, Up);
    }

    void UpdateFromStructCamera(Camera* camera) {
        Front = { camera->Front.x,camera->Front.y,camera->Front.z };
        MovementSpeed = camera->MovementSpeed;
        MouseSensitivity = camera->MouseSensitivity;
        Zoom = camera->Fovy;
        Position = { camera->Position.x,camera->Position.y,camera->Position.z };
        WorldUp = { camera->WorldUp.x,camera->WorldUp.y,camera->WorldUp.z };
        Yaw = camera->Yaw;
        Pitch = camera->Pitch;
    }

    // processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
    void ProcessKeyboard(Camera_Movement direction, float deltaTime)
    {
        float velocity = MovementSpeed * deltaTime;

        switch (Type)
        {
        case 0:
            if (direction == FORWARD)
                Position += Front * velocity;
            if (direction == BACKWARD)
                Position -= Front * velocity;
            if (direction == LEFT)
                Position -= Right * velocity;
            if (direction == RIGHT)
                Position += Right * velocity;
            break;

        case 1:
        {
            glm::vec3 flatFront = glm::normalize(glm::vec3(Front.x, 0.0f, Front.z));
            glm::vec3 flatRight = glm::normalize(glm::vec3(Right.x, 0.0f, Right.z));

            if (direction == FORWARD)
                Position += flatFront * velocity;
            if (direction == BACKWARD)
                Position -= flatFront * velocity;
            if (direction == LEFT)
                Position -= flatRight * velocity;
            if (direction == RIGHT)
                Position += flatRight * velocity;
        }
        break;

        case 2: // Editor camera with full 3D movement
            if (direction == FORWARD)
                Position += Front * velocity;
            if (direction == BACKWARD)
                Position -= Front * velocity;
            if (direction == LEFT)
                Position -= Right * velocity;
            if (direction == RIGHT)
                Position += Right * velocity;
            if (direction == UP)
                Position += Up * velocity;
            if (direction == DOWN)
                Position -= Up * velocity;
            break;

        default:
            std::cerr << "Unknown camera mode: " << Type << std::endl;
            break;
        }
    }

    // processes input received from a mouse input system. Expects the offset value in both the x and y direction.
    void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true)
    {
        xoffset *= MouseSensitivity;
        yoffset *= MouseSensitivity;

        Yaw += xoffset;
        Pitch += yoffset;

        // make sure that when pitch is out of bounds, screen doesn't get flipped
        if (constrainPitch)
        {
            if (Pitch > 89.0f)
                Pitch = 89.0f;
            if (Pitch < -89.0f)
                Pitch = -89.0f;
        }

        // update Front, Right and Up Vectors using the updated Euler angles
        updateCameraVectors();
    }

    // processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
    void ProcessMouseScroll(float yoffset)
    {
        if (scrollEnabled) {
            Zoom -= (float)yoffset;
            if (Zoom < ZoomMin)
                Zoom = ZoomMin;
            if (Zoom > ZoomMax)
                Zoom = ZoomMax;
        }
    }

    // New method for processing zoom controls with keyboard (+ and - keys)
    void ProcessZoomKeyboard(bool zoomIn, float deltaTime)
    {
        if (scrollEnabled) {
            float zoomSpeed = 30.0f * deltaTime; // Adjust zoom speed as needed
            if (zoomIn) {
                Zoom += zoomSpeed;
            }
            else {
                Zoom -= zoomSpeed;
            }

            // Clamp zoom to min/max values
            if (Zoom < ZoomMin)
                Zoom = ZoomMin;
            if (Zoom > ZoomMax)
                Zoom = ZoomMax;
        }
    }

private:
    // calculates the front vector from the Camera's (updated) Euler Angles
    void updateCameraVectors()
    {
        // calculate the new Front vector
        glm::vec3 front;
        front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        front.y = sin(glm::radians(Pitch));
        front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        Front = glm::normalize(front);
        // also re-calculate the Right and Up vector
        Right = glm::normalize(glm::cross(Front, WorldUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
        Up = glm::normalize(glm::cross(Right, Front));
    }
};
#endif