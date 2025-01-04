#ifndef LIGHT_HPP
#define LIGHT_HPP

#include "../../glm.hpp"
#include "GameObject.hpp"

/**
 * Exercise for the student: implement the Light class...
 * Imagine things like setLightType, setLightIntensity, setFalloffAngle etc, see the corresponding lectures
 */
class Light : public GameObject
{
	public:
		Light(const std::string& aName, const glm::vec3& aPosition = glm::vec3( 2.0f, 10.0f, 5.0f ));
		virtual ~Light();

		static float GetAmbientIntensity();
		static void SetAmbientIntensity(float i);
	    static glm::vec3 GetAmbientColor();
		static void SetAmbientColor(glm::vec3 clr);
        //override set parent to register/deregister light...
        virtual void _setWorldRecursively (World* pWorld) override;

		glm::vec3 Color;
		float Intensity;

private:
	static float _ambientIntensity;
	static glm::vec3 _ambientColor;
};

#endif // LIGHT_HPP
