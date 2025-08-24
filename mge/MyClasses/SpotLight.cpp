//
// Created by Birka on 24/08/2025.
//

#include "SpotLight.h"

SpotLight::SpotLight(
    const std::string &pName,
    const glm::vec3 &pPosition,
    glm::vec3 direction,
    float linearAttenuation,
    float cutoffAngleRad
    ) : Light(pName, pPosition)
{
    _direction = glm::normalize(direction);
    _linearAttenuation = linearAttenuation;
    _cutoffAngleRad = cutoffAngleRad;
}

float SpotLight::GetLinearAttenuation() const {
    return _linearAttenuation;
}

float SpotLight::GetCutoffAngleRadians() const {
    return _cutoffAngleRad;
}

glm::vec3 SpotLight::GetDirection() const {
    return _direction;
}

SpotLight::~SpotLight() {
}
