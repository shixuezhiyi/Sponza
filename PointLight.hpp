#pragma once

#include "Model.hpp"


namespace LightDefaultParameters
{
    //别问,问就是 roll 出来的
    const glm::vec3 POSITION{-9.98629f, 15.9951f, -2.02774f};
    const glm::vec3 DIRECTION{0.571362, -0.816137, 0.0864038};
    const glm::vec3 UP{0.806962, 0.577859, 0.122032};
    const glm::vec3 INTENSITY{1.8f, 1.8f, 1.8f};
}


//平行光源
class DirectionLight
{
private:
    glm::vec3 direction_;
    glm::vec3 pos_;
    glm::vec3 intensity_;
    glm::vec3 up_;
public:
    DirectionLight(glm::vec3 direction = LightDefaultParameters::DIRECTION,
                   glm::vec3 pos = LightDefaultParameters::POSITION,
                   glm::vec3 intensity = LightDefaultParameters::INTENSITY,
                   glm::vec3 up = LightDefaultParameters::UP)
            : direction_(direction), pos_(pos), intensity_(intensity), up_(up)
    {}

    glm::mat4 GetViewMatrix() const
    {
        return glm::lookAt(pos_, pos_ + direction_, up_);
    }

    glm::mat4
    GetProjectionMatrix(const float left, const float right, const float top, const float bottom, const float near,
                        const float far) const
    {
        return glm::ortho(left, right, bottom, top, near, far);
    }

    void bind(Shader &shader)
    {
        shader.setUniform("lightDir", direction_);
        shader.setUniform("lightColor", intensity_);
    }
};


class PointLight
{
private:
    glm::vec3 direction_;
    glm::vec3 pos_;
    glm::vec3 intensity_;
    glm::vec3 up_;
public:
    PointLight(glm::vec3 direction = LightDefaultParameters::DIRECTION,
               glm::vec3 pos = LightDefaultParameters::POSITION,
               glm::vec3 intensity = LightDefaultParameters::INTENSITY,
               glm::vec3 up = LightDefaultParameters::UP)
            : direction_(direction), pos_(pos), intensity_(intensity), up_(up)
    {}

    glm::mat4 GetViewMatrix() const
    {
        return glm::lookAt(pos_, pos_ + direction_, up_);
    }

    glm::mat4
    GetProjectionMatrix(const float left, const float right, const float top, const float bottom, const float near,
                        const float far) const
    {
        return glm::ortho(left, right, bottom, top, near, far);
    }

    void bind(Shader &shader)
    {
        shader.setUniform("lightPos", pos_);
        shader.setUniform("lightColor", intensity_);
    }
};