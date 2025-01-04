//
// Created by Birka on 09/12/2024.
//

#include "PointLight.h"

PointLight::PointLight(const std::string &pName, const glm::vec3 &pPosition, float linearAttenuation = 1.0f)
: Light(pName, pPosition),
_linearAttenuation(linearAttenuation)
{
}

PointLight::~PointLight() = default;

float PointLight::getAttenuation() const{
    return _linearAttenuation;
}

void PointLight::setAttenuation(const float attenuation) {
    _linearAttenuation = attenuation;
}
