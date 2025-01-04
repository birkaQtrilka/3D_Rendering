//
// Created by Birka on 10/12/2024.
//

#ifndef SINMOVE_H
#define SINMOVE_H
#include <glm/vec3.hpp>

#include "../behaviours/AbstractBehaviour.hpp"


class SinMove : public AbstractBehaviour{

private:
    float _amplitude;
    float _currTime;
    float _frequency;
public:

    SinMove(float amplitude, float frequency, glm::vec3 axis, glm::vec3 pivot);
    virtual ~SinMove();

    virtual void update(float pStep);

private:
    glm::vec3 _axis;
    glm::vec3 _pivot;
};



#endif //SINMOVE_H
