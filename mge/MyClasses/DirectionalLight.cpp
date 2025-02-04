//
// Created by Birka on 11/12/2024.
//

#include "DirectionalLight.h"

DirectionalLight::DirectionalLight(const std::string &pName, const glm::vec3 &pPosition, const glm::vec3 &direction)
: Light(pName, pPosition), Direction(glm::normalize(direction))
{

}

DirectionalLight::DirectionalLight(): Light("DirLight", glm::vec3(0, 0, 0)),
Direction(glm::vec3(1, 0, 0))
{
}

DirectionalLight::~DirectionalLight() = default;
