#include "KeysBehaviour.hpp"
#include "../core/GameObject.hpp"
#include <SFML/Window/Keyboard.hpp>

KeysBehaviour::KeysBehaviour(Mesh* pMesh, AbstractMaterial* pMat, AbstractMaterial* pMat2, float pMoveSpeed, float pTurnSpeed): AbstractBehaviour(), _moveSpeed(pMoveSpeed), _turnSpeed(pTurnSpeed)
{
	cube = new GameObject ("cubenest", glm::vec3(-2,-1,2));
	cube->scale(glm::vec3(.2f));
	cube->setMesh(pMesh);
	cube->setMaterial(pMat);
	cube->rotate(45,glm::vec3(0,1,0));

	cube2 = new GameObject ("cubenest2", glm::vec3(2,-1,2));
	cube2->scale(glm::vec3(.2f));
	cube2->setMesh(pMesh);
	cube2->setMaterial(pMat2);
	cube2->rotate(90,glm::vec3(0,1,0));
}

KeysBehaviour::~KeysBehaviour()
{
	//delete cube;
	//delete cube2;

}

void KeysBehaviour::update( float pStep )
{
	if (!init && _owner) {
		_owner->add(cube);
		_owner->add(cube2);
		init = true;
	}

	float moveSpeed = 0.0f; //default if no keys
	float turnSpeed = 0.0f;

	if ( sf::Keyboard::isKeyPressed( sf::Keyboard::W )) {
		moveSpeed = _moveSpeed;
	}
	if ( sf::Keyboard::isKeyPressed( sf::Keyboard::S )) {
		moveSpeed = -_moveSpeed;
	}
	if ( sf::Keyboard::isKeyPressed( sf::Keyboard::D )) {
		turnSpeed = -_turnSpeed;
	}
	if ( sf::Keyboard::isKeyPressed( sf::Keyboard::A )) {
		turnSpeed = +_turnSpeed;
	}
	//translate the object in its own local space
	_owner->translate( glm::vec3(0.0f, 0.0f, moveSpeed*pStep ) );

	//we can also translate directly, basically we take the z axis from the matrix
	//which is normalized and multiply it by moveSpeed*step, then we add it to the
	//translation component
	//glm::mat4 transform = _owner->getTransform();
	//transform[3] += transform[2] * moveSpeed*pStep;
	//_owner->setTransform(transform);

	//rotate the object in its own local space
	_owner->rotate( glm::radians(turnSpeed*pStep), glm::vec3(0.0f, 1.0f, 0.0f ) );

	//NOTE:
	//The reason the above happens in the local space of the object and not in the world space
	//is that we use the _owner->translate / rotate shortcuts which in turn call glm::rotate / glm::translate
	//The methods multiply the current transform with the translation/rotation matrix from left to right
	//meaning everything happens in local space.
}
