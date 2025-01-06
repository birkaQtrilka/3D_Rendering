//
// Created by Birka on 04/01/2025.
//

#ifndef OCEANMAT_H
#define OCEANMAT_H

#include <GL/glew.h>
#include <glm/fwd.hpp>
#include <vector>
#include <SFML/System/Clock.hpp>

#include "../../materials/AbstractMaterial.hpp"
class PointLight;
class ShaderProgram;
class Texture;

class OceanMat : public AbstractMaterial
{
public:
    OceanMat(glm::vec3 color);
    virtual ~OceanMat();

    float GetSpecularIntensity() const;
    void SetSpecularIntensity(float specularIntensity);
    float GetSpecularPower() const;
    void SetSpecularPower(float pSpecularPower);
    float WaveAmplitude;
    float WaveLength;
    float WaveSpeed;

private:
    float _specularIntensity;
    float _specularPower;
    float _time;
    sf::Clock updateClock;
    sf::Time timeEllapsed;

    glm::vec3 _diffuseColor;

    virtual void render(World* pWorld, Mesh* pMesh, const glm::mat4& pModelMatrix, const glm::mat4& pViewMatrix, const glm::mat4& pProjectionMatrix) override;

    static void _lazyInitializeShader();

    static GLuint _uMMatrix;
    static ShaderProgram* _shader;
    static GLint _aVertex ;
    static GLint _aNormal;
    static GLint _aUV ;
    static GLint _uMVPMatrix ;
};



#endif //OCEANMAT_H
