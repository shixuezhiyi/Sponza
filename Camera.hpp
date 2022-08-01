#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods


// Default camera values
namespace CameraDefaultParameters
{
    const float YAW = -90.0f;
    const float PITCH = 0.0f;
    const float SPEED = 2.5f;
    const float SENSITIVITY = 0.1f;
    const float ZOOM = 45.0f;
    const glm::vec3 POSITION = glm::vec3(0.0f, 0.0f, 0.0f);
    const glm::vec3 UP = glm::vec3(0.0f, 1.0f, 0.0f);
    const glm::vec3 FRONT = glm::vec3(0.0f, 0.0f, -1.0f);

    enum Camera_Movement
    {
        FORWARD,
        BACKWARD,
        LEFT,
        RIGHT
    };
}

// An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
class Camera
{
private:
    // camera Attributes
    glm::vec3 pos_;
    glm::vec3 front_;
    glm::vec3 up_;
    glm::vec3 right_;
    glm::vec3 worldUp_;
    // euler Angles
    float yaw_;
    float pitch_;
    // camera options
    float movementSpeed_;
    float mouseSensitivity_;
    float zoom_;
public:
    // constructor with vectors
    Camera()
    {
        pos_ = CameraDefaultParameters::POSITION;
        worldUp_ = CameraDefaultParameters::UP;
        yaw_ = CameraDefaultParameters::YAW;
        pitch_ = CameraDefaultParameters::PITCH;
        front_ = CameraDefaultParameters::FRONT;
        movementSpeed_ = CameraDefaultParameters::SPEED;
        mouseSensitivity_ = CameraDefaultParameters::SENSITIVITY;
        zoom_ = CameraDefaultParameters::ZOOM;
        updateCameraVectors();
    }

    Camera(glm::vec3 pos)
    {
        pos_ = pos;
        worldUp_ = CameraDefaultParameters::UP;
        yaw_ = CameraDefaultParameters::YAW;
        pitch_ = CameraDefaultParameters::PITCH;
        front_ = CameraDefaultParameters::FRONT;
        movementSpeed_ = CameraDefaultParameters::SPEED;
        mouseSensitivity_ = CameraDefaultParameters::SENSITIVITY;
        zoom_ = CameraDefaultParameters::ZOOM;
        updateCameraVectors();
    }

    // constructor with scalar values
    Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch)
    {
        front_ = CameraDefaultParameters::FRONT;
        movementSpeed_ = CameraDefaultParameters::SPEED;
        mouseSensitivity_ = CameraDefaultParameters::SENSITIVITY;
        zoom_ = CameraDefaultParameters::ZOOM;
        pos_ = glm::vec3(posX, posY, posZ);
        worldUp_ = glm::vec3(upX, upY, upZ);
        yaw_ = yaw;
        pitch_ = pitch;
        updateCameraVectors();
    }

    // returns the view matrix calculated using Euler Angles and the LookAt Matrix
    glm::mat4 GetViewMatrix() const
    {
        return glm::lookAt(pos_, pos_ + front_, up_);
    }

    glm::mat4 GetProjectionMatrix(const float aspect, const float zNear, const float zFar) const
    {
        return glm::perspective(zoom_, aspect, zNear, zFar);
    }

    // processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
    void ProcessKeyboard(CameraDefaultParameters::Camera_Movement direction, float deltaTime)
    {
        float velocity = movementSpeed_ * deltaTime;
        if (direction == CameraDefaultParameters::FORWARD)
            pos_ += front_ * velocity;
        if (direction == CameraDefaultParameters::BACKWARD)
            pos_ -= front_ * velocity;
        if (direction == CameraDefaultParameters::LEFT)
            pos_ -= right_ * velocity;
        if (direction == CameraDefaultParameters::RIGHT)
            pos_ += right_ * velocity;
    }

    // processes input received from a mouse input system. Expects the offset value in both the x and y direction.
    void ProcessMouseMovement(float xOffset, float yOffset, GLboolean constrainPitch = true)
    {
        xOffset *= mouseSensitivity_;
        yOffset *= mouseSensitivity_;

        yaw_ += xOffset;
        pitch_ += yOffset;

        // make sure that when pitch is out of bounds, screen doesn't get flipped
        if (constrainPitch)
        {
            if (pitch_ > 89.0f)
                pitch_ = 89.0f;
            if (pitch_ < -89.0f)
                pitch_ = -89.0f;
        }

        // update Front, Right and Up Vectors using the updated Euler angles
        updateCameraVectors();
    }

    // processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
    void ProcessMouseScroll(float yOffset)
    {
        zoom_ -= (float) yOffset;
        if (zoom_ < 1.0f)
            zoom_ = 1.0f;
        if (zoom_ > 45.0f)
            zoom_ = 45.0f;
    }

    void output() const
    {
        std::cout << "Camera position: " << pos_.x << " " << pos_.y << " " << pos_.z << std::endl;
        std::cout << "Camera front: " << front_.x << " " << front_.y << " " << front_.z << std::endl;
        std::cout << "Camera up: " << up_.x << " " << up_.y << " " << up_.z << std::endl;
    }

private:
    // calculates the front vector from the Camera's (updated) Euler Angles
    void updateCameraVectors()
    {
        // calculate the new Front vector
        glm::vec3 front;
        front.x = cos(glm::radians(yaw_)) * cos(glm::radians(pitch_));
        front.y = sin(glm::radians(pitch_));
        front.z = sin(glm::radians(yaw_)) * cos(glm::radians(pitch_));
        front_ = glm::normalize(front);
        // also re-calculate the Right and Up vector
        right_ = glm::normalize(glm::cross(front_,
                                           worldUp_));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
        up_ = glm::normalize(glm::cross(right_, front_));
    }
};
