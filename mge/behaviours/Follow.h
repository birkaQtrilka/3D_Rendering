//
// Created by Birka on 25/11/2024.
//

#ifndef FOLLOW_H
#define FOLLOW_H
#include <glm/fwd.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include "AbstractBehaviour.hpp"
#include <glm/gtc/matrix_transform.hpp>
class Camera;

class Follow : public AbstractBehaviour{
private:
    glm::vec2 _lastMousePos;
    glm::vec3 _orbitPos;
    glm::vec3 _prevPos;
    GameObject* _parent;
    bool _initialised;
    Camera* _camera;
public:
    //glm::vec3 Offset;
    float OrbitDistance;

    GameObject* Target;

    Follow(float orbitDistance, GameObject* target, Camera* camera);
    virtual ~Follow();

    virtual void update(float pStep);

};



#endif //FOLLOW_H
