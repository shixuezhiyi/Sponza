#pragma once

#include "Model.hpp"


namespace LightDefaultParameters
{
    //别问,问就是 roll 出来的
    const glm::vec3 POSITION{0.0f, 5.5f, 0.0f};
    const glm::vec3 DIRECTION{0.571362, -0.816137, 0.0864038};
    const glm::vec3 UP{0.806962, 0.577859, 0.122032};
    const glm::vec3 INTENSITY{15.f, 15.f, 15.f};
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
    {
    }

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
    glm::vec3 pos_;
    glm::vec3 intensity_;
    MyModel sphere;
    bool isVisible_;

public:
    PointLight(glm::vec3 pos = LightDefaultParameters::POSITION,
               glm::vec3 intensity = LightDefaultParameters::INTENSITY,
               const string &spherePath = "sphere/scene.gltf")
            : pos_(pos), intensity_(intensity), sphere(MyModel(spherePath)),
              isVisible_(false)
    {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, pos);
        model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
        sphere.setModelMat(model);
    }


    void bind(Shader &shader)
    {
        shader.setUniform("lightPos", pos_);
        shader.setUniform("lightColor", intensity_);
    }

    auto getPos()
    {
        return pos_;
    }

    void setVisible(bool isVisible)
    {
        isVisible_ = isVisible;
    }

    void setVisible()
    {
        isVisible_ = !isVisible_;
    }

    void draw(Shader &shader)
    {
        if (isVisible_)
            sphere.draw(shader);
    }


    //cube shadow map 变换矩阵 right left top bottom near far
    std::vector<glm::mat4> getShadowTransforms(glm::mat4 &shadowProj) const
    {
        std::vector<glm::mat4> shadowTransforms;
        shadowTransforms.push_back(
                shadowProj * glm::lookAt(pos_, pos_ + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
        shadowTransforms.push_back(
                shadowProj * glm::lookAt(pos_, pos_ + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
        shadowTransforms.push_back(
                shadowProj * glm::lookAt(pos_, pos_ + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)));
        shadowTransforms.push_back(
                shadowProj * glm::lookAt(pos_, pos_ + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0)));
        shadowTransforms.push_back(
                shadowProj * glm::lookAt(pos_, pos_ + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0)));
        shadowTransforms.push_back(
                shadowProj * glm::lookAt(pos_, pos_ + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0)));
        return shadowTransforms;
    }
};