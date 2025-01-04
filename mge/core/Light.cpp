#include "Light.hpp"
#include "World.hpp"

Light::Light(const std::string& pName, const glm::vec3& pPosition):GameObject(pName, pPosition),
Color(glm::vec3(1.0f, 1.0f, 1.0f)), Intensity(1)
{}

Light::~Light() {
}

float Light::_ambientIntensity = .9f;
glm::vec3 Light::_ambientColor = glm::vec3(.2, .2, .2);

float Light::GetAmbientIntensity() {
	return Light::_ambientIntensity;
}

void Light::SetAmbientIntensity(float i) {
	Light::_ambientIntensity = i;
}

glm::vec3 Light::GetAmbientColor() {
	return Light::_ambientColor;
}

void Light::SetAmbientColor(glm::vec3 clr) {
	Light::_ambientColor = clr;
}

//Override setWorldRecursively to make sure we are registered
//to the world we are a part of.
void Light::_setWorldRecursively (World* pWorld) {

    //store the previous and new world before and after storing the given world
    World* previousWorld = _world;
    GameObject::_setWorldRecursively(pWorld);
    World* newWorld = _world;

    //check whether we need to register or unregister
    if (previousWorld != nullptr) previousWorld->unregisterLight(this);
	if (newWorld != nullptr) newWorld->registerLight(this);

}


