//
// Created by Birka on 24/08/2025.
//

#ifndef SPOTLIGHT_H
#define SPOTLIGHT_H

#include "../core/Light.hpp"

class SpotLight final : public Light{
public:
    SpotLight(
        const std::string& pName,
        const glm::vec3& pPosition,
        glm::vec3 direction,
        float linearAttenuation,
        float cutoffAngleRad
    );
    float GetLinearAttenuation() const;
    float GetCutoffAngleRadians() const;
    glm::vec3 GetDirection() const;

    ~SpotLight();
private:
    float _linearAttenuation;
    float _cutoffAngleRad;
    glm::vec3 _direction{};
};


#endif //SPOTLIGHT_H
