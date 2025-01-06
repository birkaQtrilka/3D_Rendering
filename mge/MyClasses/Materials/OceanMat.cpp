//
// Created by Birka on 04/01/2025.
//

#include "OceanMat.h"
#include "TerrainMat.h"
#include <GL/glew.h>

#include <utility>
#include "../../core/Texture.hpp"
#include "../../../glm.hpp"

#include "../DirectionalLight.h"
#include "../../config.hpp"
#include "../../core/Camera.hpp"
#include "../../core/GameObject.hpp"
#include "../../core/Light.hpp"
#include "../../core/Mesh.hpp"
#include "../../core/ShaderProgram.hpp"
#include "../../core/World.hpp"

GLint OceanMat::_aVertex = 0;
GLint OceanMat::_aNormal = 0;
GLint OceanMat::_aUV = 0;
GLint OceanMat::_uMVPMatrix = 0;
GLuint OceanMat::_uMMatrix = 0;
ShaderProgram* OceanMat::_shader = NULL;


OceanMat::OceanMat(glm::vec3 color) : WaveAmplitude(.5f), WaveLength(2.0f),
WaveSpeed(.1f), _specularIntensity(0), _specularPower(1), _time(0),
_diffuseColor(color)
{
    _lazyInitializeShader();
}

OceanMat::~OceanMat() = default;

float OceanMat::GetSpecularIntensity() const {
    return _specularIntensity;
}

void OceanMat::SetSpecularIntensity(const float pSpecularIntensity) {
    _specularIntensity = pSpecularIntensity;
}

float OceanMat::GetSpecularPower() const {
    return _specularPower;
}

void OceanMat::SetSpecularPower(const float pSpecularPower) {
    _specularPower = pSpecularPower;
}

void OceanMat::_lazyInitializeShader() {
    if (!_shader) {
        _shader = new ShaderProgram();
        _shader->addShader(GL_VERTEX_SHADER, config::MGE_SHADER_PATH + "water.vs");
        _shader->addShader(GL_FRAGMENT_SHADER, config::MGE_SHADER_PATH + "water.fs");
        _shader->finalize();

        _uMVPMatrix = _shader->getUniformLocation("mvpMatrix");
        _uMMatrix = _shader->getUniformLocation("mMatrix");

        _aVertex = _shader->getAttribLocation("vertex");
        _aNormal = _shader->getAttribLocation("normal");
        _aUV = _shader->getAttribLocation("uv");

    }
}


void OceanMat::render(World *pWorld, Mesh *pMesh, const glm::mat4 &pModelMatrix, const glm::mat4 &pViewMatrix,
                               const glm::mat4 &pProjectionMatrix) {
    // if (!_heightMapTexture) {
    //     printf("no texture-------------------------------------");
    //     _heightMapTexture = Texture::load(config::MGE_TEXTURE_PATH + "default.jpg");
    //     return;
    // }
    timeEllapsed = updateClock.getElapsedTime();
    _shader->use();

    if (pWorld->getLightCount() != 0) {
        Light *light = pWorld->getLightAt(0);

        glm::vec3 ambientClr = Light::GetAmbientColor();
        glm::vec3 viewerPos = pWorld->getMainCamera()->getWorldPosition();
        glUniform3f(_shader->getUniformLocation("ambientClr"), ambientClr.x, ambientClr.y, ambientClr.z);
        glUniform1f(_shader->getUniformLocation("ambientIntensity"), Light::GetAmbientIntensity());
        glUniform4f(_shader->getUniformLocation("viewerPosV"), viewerPos.x, viewerPos.y, viewerPos.z, 1);
        glUniform1f(_shader->getUniformLocation("specularIntensity"),_specularIntensity);
        glUniform1f(_shader->getUniformLocation("specularPower"),_specularPower);
        glUniform1f(_shader->getUniformLocation("waveAmplitude"),WaveAmplitude);
        glUniform1f(_shader->getUniformLocation("waveLength"),WaveLength);
        glUniform1f(_shader->getUniformLocation("waveSpeed"),WaveSpeed);
        glUniform1f(_shader->getUniformLocation("time"),timeEllapsed.asSeconds());

        auto dirLight = dynamic_cast<DirectionalLight*>(light);
        glUniform3f(_shader->getUniformLocation("directionalLight.direction"),
                    dirLight->Direction.x,
                    dirLight->Direction.y,
                    dirLight->Direction.z
        );
        glUniform3f(_shader->getUniformLocation("directionalLight.color"),
                    dirLight->Color.x,
                    dirLight->Color.y,
                    dirLight->Color.z
        );
        glUniform1f(_shader->getUniformLocation("directionalLight.intensity"),
                    dirLight->Intensity
        );
    }

    glUniform3f(_shader->getUniformLocation("diffuseColor"),
            _diffuseColor.r,_diffuseColor.g,_diffuseColor.b
        );
    //glUniform1f(_shader->getUniformLocation("maxHeight"), _maxHeight);

    //pass in a precalculate mvp matrix (see texture material for the opposite)
    glm::mat4 mvpMatrix = pProjectionMatrix * pViewMatrix * pModelMatrix;
    glUniformMatrix4fv(_uMVPMatrix, 1, GL_FALSE, glm::value_ptr(mvpMatrix));

    glUniformMatrix4fv(_uMMatrix, 1, GL_FALSE, glm::value_ptr(pModelMatrix));

    //now inform mesh of where to stream its data
    pMesh->streamToOpenGL(
        _shader->getAttribLocation("vertex"),
        _shader->getAttribLocation("normal"),
        _shader->getAttribLocation("uv")
    );
}