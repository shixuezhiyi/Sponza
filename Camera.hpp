//
// Created by 滕逸青 on 2022/7/27.
//

#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

enum CameraAction
{
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    MOUSEMOVE
};

class Camera
{
public:
    Camera(const glm::vec3 &cameraPos = {0.0f, 0.0f, 0.0f}, const glm::vec3 &cameraTarget = {0.0f, 1.0f, 0.0f},
           const glm::vec3 &cameraUp = {0.0f, 1.0f, 0.0f}, const float &cameraSpeed = 0.25, const float &yaw = -90.0f,
           const float &pitch = 0.0f, bool constrainPitch = true, float mouseSensitivity = 0.1f) :
            _pos(cameraPos), _up(cameraUp), _speed(cameraSpeed), _target(cameraTarget), _pitch(pitch), _yaw(yaw),
            _constrainPitch(constrainPitch), _mouseSensitivity(mouseSensitivity)
    {
        cameraUpdate();
    }

    glm::mat4 getViewMat() const
    {
        auto view = glm::lookAt(_pos, _target, _up);
        return view;
    }

    void move(CameraAction action, float xOffset, float yOffset, const float &deltaTime = 0)
    {
        auto stride = deltaTime * _speed;

        if (action == FORWARD)
            _pos += _front * stride;
        if (action == BACKWARD)
            _pos -= _front * stride;
        if (action == LEFT)
            _pos += glm::normalize(glm::cross(_up, _front)) * stride;
        if (action == RIGHT)
            _pos -= glm::normalize(glm::cross(_up, _front)) * stride;
        if (action == MOUSEMOVE)
            mouseMove(xOffset, yOffset);
    }


private:
    glm::vec3 _pos;//Camera Position
    glm::vec3 _target;
    glm::vec3 _up;
    glm::vec3 _front;

    float _yaw;
    float _pitch;


    bool _constrainPitch;
    float _mouseSensitivity;
    float _speed;

    void mouseMove(float xOffset, float yOffset)
    {
        xOffset *= _mouseSensitivity;
        yOffset *= _mouseSensitivity;
        _yaw += xOffset;
        _pitch += yOffset;
        if (_constrainPitch)
        {
            _pitch = glm::clamp(_pitch, -89.0f, +89.0f);
        }
        cameraUpdate();
    }


    void cameraUpdate()
    {
        glm::vec3 front{
                cos(glm::radians(_yaw)) * cos(glm::radians(_pitch)),
                sin(glm::radians(_pitch)),
                sin(glm::radians(_yaw)) * cos(glm::radians(_pitch))
        };
        _front = glm::normalize(front);
        _target = _pos + _front;
        auto right = glm::normalize(glm::cross(_up, _front));
        _up = glm::normalize(glm::cross(_front, right));
    }
};

