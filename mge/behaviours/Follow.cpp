//
// Created by Birka on 25/11/2024.
//

#include "Follow.h"

#include <string>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>

#include "../core/GameObject.hpp"
#include "../core/Camera.hpp"

Follow::Follow(float orbitDistance, GameObject* target, Camera* camera) : AbstractBehaviour(), _lastMousePos(glm::vec2(0,0)),
_orbitPos(glm::vec3(0,0,0)), _prevPos(glm::vec3(0)), OrbitDistance(orbitDistance), _initialised(false),
_camera(camera) {
     //_orbitPos.y += -glm::pi<float>()/4;
    Target = target;

}

Follow::~Follow()
{
    //dtor
}

void Follow::update(float pStep)
{
if (_owner && !_initialised) {
    _initialised = true;
    _parent = new GameObject("pivot",Target->getWorldPosition());
    _owner->getParent()->add(_parent);
    _owner->getParent()->remove(_owner);
    _owner->setParent(_parent);
    _owner->setLocalPosition(glm::vec3(0,0,OrbitDistance));
    _prevPos = _owner->getLocalPosition();
}

    const auto mousePos = sf::Mouse::getPosition();
    glm::vec2 glmMousePos = glm::vec2(mousePos.x, mousePos.y);

    bool leftClick = sf::Mouse::isButtonPressed(sf::Mouse::Left);
    if (leftClick) {
        const glm::vec2 delta = glmMousePos - _lastMousePos;
        _orbitPos.y += -delta.y * pStep * 0.4f;
        _orbitPos.x += delta.x * pStep * 0.4f;
        _orbitPos.y = glm::clamp(_orbitPos.y, glm::radians(-45.0f),  glm::radians(45.0f));
        auto right = glm::vec4(1,0,0,0);
        auto up = glm::vec4(0,1,0,0);

        _parent->setTransform(glm::identity<glm::mat4>());
        _parent->rotate(_orbitPos.x, up);
        _parent->rotate(_orbitPos.y, right);

    }
    //auto lerpedPos = glm::mix(_parent->getWorldPosition(), Target->getWorldPosition(), pStep * 0.5f);
    auto lerpedPos = glm::mix(_prevPos, Target->getWorldPosition(), pStep * 5.0f);
    //auto lerpedPos = Target->getWorldPosition();
    _parent->setLocalPosition(lerpedPos);
    _lastMousePos = glmMousePos;
    _prevPos = lerpedPos;
    float zoomSpeed = 40.0f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q)) {
        _camera->SetFov(_camera->GetFov() + pStep * zoomSpeed);

    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::E)) {
        _camera->SetFov(_camera->GetFov() - pStep * zoomSpeed);
    }
}
