//
// Created by Birka on 11/12/2024.
//

#ifndef DIRECTIONALLIGHT_H
#define DIRECTIONALLIGHT_H

#include "../core/Light.hpp"


class DirectionalLight : public Light{
public:
    DirectionalLight(const std::string& pName, const glm::vec3& pPosition,const glm::vec3 &direction);

    DirectionalLight();

    ~DirectionalLight() override;
    glm::vec3 Direction;
};



#endif //DIRECTIONALLIGHT_H
