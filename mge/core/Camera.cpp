#include "glm/glm.hpp"
#include "Camera.hpp"

Camera::Camera( std::string pName, glm::vec3 pPosition, glm::mat4 pProjectionMatrix )
:	GameObject(pName, pPosition), _projection(pProjectionMatrix)
{
}

Camera::~Camera()
{
	//dtor
}

glm::mat4& Camera::getProjection() {
    return _projection;
}

void Camera::SetFov(float fov) {
    fov = glm::clamp(fov, 1.0f, 60.0f);
    _fov = fov;
    _projection = glm::perspective (glm::radians(fov), 4.0f/3.0f, 0.1f, 1000.0f  );

}

float Camera::GetFov() {
    return _fov;
}

