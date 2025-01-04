//
// Created by Birka on 25/11/2024.
//


#include "ColoredTextureMat.h"

#include <GL/glew.h>
#include "../../core/Texture.hpp"
#include "../../../glm.hpp"

#include "../DirectionalLight.h"
#include "../PointLight.h"
#include "../../config.hpp"
#include "../../core/Camera.hpp"
#include "../../core/GameObject.hpp"
#include "../../core/Light.hpp"
#include "../../core/Mesh.hpp"
#include "../../core/ShaderProgram.hpp"
#include "../../core/World.hpp"

// template < class T, class U >
// static bool IsInst(U u, T* result) {
//     result = dynamic_cast< T >(u);
//
//     return result != nullptr;
// }

GLint ColoredTextureMat::_uMVPMatrix = 0;
GLint ColoredTextureMat::_uMMatrix = 0;
GLint ColoredTextureMat::_uDiffuseTexture = 0;
GLint ColoredTextureMat::_aVertex = 0;
GLint ColoredTextureMat::_aNormal = 0;
GLint ColoredTextureMat::_aUV = 0;
std::vector<float> ColoredTextureMat::_lightData;
GLuint ColoredTextureMat::_lightBuffer = 0;

ShaderProgram *ColoredTextureMat::_shader = NULL;

ColoredTextureMat::ColoredTextureMat(Texture *pDiffuseTexture, glm::vec3 pDiffuseColor): _diffuseColor(pDiffuseColor),
    _diffuseTexture(pDiffuseTexture), _prevLightCount(0), SpecularIntensity(10), SpecularPower(225){
    //every time we create an instance of colormaterial we check if the corresponding shader has already been loaded
    _lazyInitializeShader();
}

ColoredTextureMat::~ColoredTextureMat() {
    //dtor
}

void ColoredTextureMat::_lazyInitializeShader() {
    if (!_shader) {
        _shader = new ShaderProgram();
        _shader->addShader(GL_VERTEX_SHADER, config::MGE_SHADER_PATH + "clrTexture.vs");
        _shader->addShader(GL_FRAGMENT_SHADER, config::MGE_SHADER_PATH + "clrTexture.fs");
        _shader->finalize();

        //cache all the uniform and attribute indexes
        _uMVPMatrix = _shader->getUniformLocation("mvpMatrix");
        _uMMatrix = _shader->getUniformLocation("mMatrix");
        _uDiffuseTexture = _shader->getUniformLocation("diffuseTexture");

        _aVertex = _shader->getAttribLocation("vertex");
        _aNormal = _shader->getAttribLocation("normal");
        _aUV = _shader->getAttribLocation("uv");

        glGenBuffers(1, &_lightBuffer);
        glBindBuffer(GL_UNIFORM_BUFFER, _lightBuffer);
        glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(float) * 8, _lightData.begin()._Ptr, GL_STATIC_DRAW);
    }
}



void ColoredTextureMat::setDiffuseColor(glm::vec3 pDiffuseColor) {
    _diffuseColor = pDiffuseColor;
}

void ColoredTextureMat::render(World *pWorld, Mesh *pMesh, const glm::mat4 &pModelMatrix, const glm::mat4 &pViewMatrix,
                               const glm::mat4 &pProjectionMatrix) {
    if (!_diffuseTexture) {
        printf("no texture-------------------------------------");
        _diffuseTexture = Texture::load(config::MGE_TEXTURE_PATH + "default.jpg");
        return;
    }

    _shader->use();
    _lightData.clear();
    if (pWorld->getLightCount() != 0) {
        glm::vec3 ambientClr = Light::GetAmbientColor();
        glm::vec3 viewerPos = pWorld->getMainCamera()->getWorldPosition();
        glUniform3f(_shader->getUniformLocation("ambientClr"), ambientClr.x, ambientClr.y, ambientClr.z);
        glUniform1f(_shader->getUniformLocation("ambientIntensity"), Light::GetAmbientIntensity());
        glUniform4f(_shader->getUniformLocation("viewerPosV"), viewerPos.x, viewerPos.y, viewerPos.z, 1);
        glUniform1f(_shader->getUniformLocation("specularIntensity"),SpecularIntensity);
        glUniform1f(_shader->getUniformLocation("specularPower"),SpecularPower);

        for (int i = 0; i < pWorld->getLightCount(); i++) {
            Light *light = pWorld->getLightAt(i);

            auto pLight = dynamic_cast<PointLight *>(light);


            if (pLight != nullptr) {
                AddPointLight(pLight);
                glBindBufferBase(GL_UNIFORM_BUFFER, 1, _lightBuffer);
            } else {
                auto dirLight = dynamic_cast<DirectionalLight *>(light);
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
        }

        glUniform1i(_shader->getUniformLocation("lightCount"), _lightData.size() / 8);

        //is doing this every frame a good idea?
        glBufferSubData(GL_UNIFORM_BUFFER, 0, _lightData.size() * sizeof(float), _lightData.begin()._Ptr);
        //OnLigthAdded();
        //this is not needed?
        // if (_prevLightCount != pWorld->getLightCount())
        // {
        //     std::cout << "Lights have different amount of lights" << std::endl;
        //     _prevLightCount = pWorld->getLightCount();
        // }
    }

    glUniform3f(_shader->getUniformLocation("diffuseColor"),
        _diffuseColor.r, _diffuseColor.g, _diffuseColor.b
    );

    //setup texture slot 0
    glActiveTexture(GL_TEXTURE0);
    //bind the texture to the current active slot
    glBindTexture(GL_TEXTURE_2D, _diffuseTexture->getId());
    //tell the shader the texture slot for the diffuse texture is slot 0
    glUniform1i(_uDiffuseTexture, 0);

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

void ColoredTextureMat::OnLigthAdded() {
    glBufferSubData(GL_UNIFORM_BUFFER, 0, _lightData.size() * sizeof(float), _lightData.begin()._Ptr);
}

void ColoredTextureMat::AddPointLight(PointLight *l) {
    auto pos = l->getWorldPosition();
    //if (pos.x==0)
    //std::cout << pos << std::endl;
    //pos
    _lightData.push_back(pos.x);
    _lightData.push_back(pos.y);
    _lightData.push_back(pos.z);
    //intensity
    _lightData.push_back(l->Intensity);
    auto clr = l->Color;
    //clr
    _lightData.push_back(clr.r);
    _lightData.push_back(clr.g);
    _lightData.push_back(clr.b);
    //attenuation
    _lightData.push_back(l->getAttenuation());
}
