//
// Created by Birka on 25/08/2025.
//

#include "RotateDirection.h"

#include "../MyClasses/SpotLight.h"

RotateDirection::RotateDirection(SpotLight* spotLight, float speed, float radius)
        : _spotLight(spotLight), _speed(speed), _radius(radius) {}

RotateDirection::~RotateDirection() {
}

void RotateDirection::update(float pStep) {
    _angle += _speed * pStep;   // increment angle based on delta time

    // keep angle in [0, 2π] for numerical stability
    if (_angle > 2.0f * glm::pi<float>()) _angle -= 2.0f * glm::pi<float>();

    // compute new direction vector
    glm::vec3 dir;
    dir.x = cos(_angle) * _radius;
    dir.y = -1.0f;       // pointing down towards the floor
    dir.z = sin(_angle) * _radius;

    dir = glm::normalize(dir);   // normalize to unit vector

    _spotLight->SetDirection(dir);
}
