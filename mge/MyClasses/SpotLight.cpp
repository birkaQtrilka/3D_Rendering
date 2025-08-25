//
// Created by Birka on 24/08/2025.
//

#include "SpotLight.h"

SpotLight::SpotLight(
    const std::string &pName,
    const glm::vec3 &pPosition,
    glm::vec3 direction,
    float linearAttenuation,
    float cutoffAngleRad,
    float innerCutoffAngleRad
    ) : Light(pName, pPosition)
{
    _direction = glm::normalize(direction);
    _linearAttenuation = linearAttenuation;
    _cutoffAngleRad = cutoffAngleRad;
    _innerCutoffAngleRad = innerCutoffAngleRad;
    // _cutoffAngleRad = glm::fmax(0, cutoffAngleRad);
    // _innerCutoffAngleRad = glm::min(innerCutoffAngleRad, _cutoffAngleRad);
}

float SpotLight::GetLinearAttenuation() const {
    return _linearAttenuation;
}

float SpotLight::GetCutoffAngleRadians() const {
    return _cutoffAngleRad;
}

float SpotLight::GetInnerCutoffAngleRadians() const {
    return _innerCutoffAngleRad;
}

glm::vec3 SpotLight::GetDirection() const {
    return glm::vec4(_direction,1) ;
}

void SpotLight::SetDirection(const glm::vec3 direction) {
    _direction = glm::normalize(direction);
}

SpotLight::~SpotLight() {
}
