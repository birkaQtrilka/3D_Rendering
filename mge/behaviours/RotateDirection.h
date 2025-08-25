//
// Created by Birka on 25/08/2025.
//

#ifndef ROTATEDIRECTION_H
#define ROTATEDIRECTION_H
#include "AbstractBehaviour.hpp"

class SpotLight;

class RotateDirection : public AbstractBehaviour {
public:
    explicit RotateDirection(SpotLight* spotLight, float speed = 1.0f, float radius = 1.0f);

    ~RotateDirection() override;

    void update(float pStep) override;
private:
    SpotLight* _spotLight;
    float _angle = 0.0f;
    float _speed = 1.0f;
    float _radius = 1.0f;
};



#endif //ROTATEDIRECTION_H
