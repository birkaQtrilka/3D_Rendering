//
// Created by Birka on 09/12/2024.
//

#ifndef POINTLIGHT_H
#define POINTLIGHT_H


#include "../core/Light.hpp"

class PointLight : public Light{
public:
    PointLight(const std::string& pName, const glm::vec3& pPosition, float linearAttenuation);
    ~PointLight();
    float getAttenuation() const;
    void setAttenuation(const float attenuation);
private:
    float _linearAttenuation;
};



#endif //POINTLIGHT_H
