//
// Created by Birka on 10/12/2024.
//

#include "SinMove.h"

#include "../core/GameObject.hpp"

SinMove::SinMove(float amplitude, float frequency, glm::vec3 axis, glm::vec3 pivot)
: _frequency(frequency), _amplitude(amplitude), _currTime(0), _pivot(pivot), _axis(axis)
{

}

SinMove::~SinMove() = default;

void SinMove::update(float pStep) {
    //auto l = _owner->getLocalPosition();
    _currTime += pStep;

    auto t = glm::translate(_owner->getTransform(), _pivot);
    auto rotation = glm::rotate( t, pStep * _frequency, _axis);
    rotation = glm::translate( rotation, -_pivot);

    _owner->setTransform(rotation);
    //_owner->setLocalPosition(glm::vec3(glm::sin(_currTime * _frequency) * _amplitude,l.y,glm::cos(_currTime * _frequency)));
}
