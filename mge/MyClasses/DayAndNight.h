//
// Created by Birka on 04/02/2025.
//

#ifndef DAYANDNIGHT_H
#define DAYANDNIGHT_H
#include <glm/vec3.hpp>

#include "DirectionalLight.h"
#include "../behaviours/AbstractBehaviour.hpp"


class DayAndNight : public AbstractBehaviour{

public:
    float RotationSpeed;
    DayAndNight(DirectionalLight& directionalLight, glm::vec3 rotationAxis ,glm::vec3 minColor, glm::vec3 maxColor);
    ~DayAndNight();
    virtual void update(float pStep);

    glm::vec3 MinColor;
    glm::vec3 MaxColor;
    glm::vec3 RotationAxis;
private:
    DirectionalLight* _directionalLight;
    glm::mat4x4 _rotMatrix;
};



#endif //DAYANDNIGHT_H
