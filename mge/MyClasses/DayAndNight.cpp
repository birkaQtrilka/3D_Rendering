//
// Created by Birka on 04/02/2025.
//

#include "DayAndNight.h"

DayAndNight::DayAndNight( DirectionalLight& directionalLight, glm::vec3 rotationAxis, glm::vec3 minColor, glm::vec3 maxColor):
    RotationSpeed(1), _rotMatrix(glm::mat4(1)), RotationAxis(rotationAxis),
    _directionalLight(), MinColor(minColor), MaxColor(maxColor) {
    _directionalLight = &directionalLight;

    _rotMatrix = glm::rotate(glm::mat4(1), glm::radians(90.0f) , glm::vec3(0,1,0));
}

DayAndNight::~DayAndNight() = default;

void DayAndNight::update(float pStep) {
    glm::vec3 down = glm::vec3(0, 1, 0);
    _rotMatrix = glm::rotate(_rotMatrix, RotationSpeed * pStep, RotationAxis);
    glm::vec3 direction = _rotMatrix * glm::vec4(0,0,1,0);
    float dot = glm::max( 0.0f, glm::dot(down, direction));
    float ease = 1.0f - glm::pow(1 - dot, 4);
    float rAverage = MinColor.r + (MaxColor.r - MinColor.r) * ease;
    float gAverage = MinColor.g + (MaxColor.g - MinColor.g) * ease;
    float bAverage = MinColor.b + (MaxColor.b - MinColor.b) * ease;
    std::cout << "dot: " << ease << std::endl;
    _directionalLight->Color = glm::vec3(rAverage, gAverage, bAverage);
    _directionalLight->Direction = direction;
}
