//
// Created by Birka on 25/11/2024.
//


#include "ColoredTextureMat.h"

#include <GL/glew.h>
#include "../../core/Texture.hpp"
#include "../../../glm.hpp"

#include "../DirectionalLight.h"
#include "../PointLight.h"
#include "../SpotLight.h"
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
std::vector<float> ColoredTextureMat::_pointLightData;
std::vector<float> ColoredTextureMat::_spotLightData;
GLuint ColoredTextureMat::_pointLightBufferID = 0;
GLuint ColoredTextureMat::_spotLightBufferID = 0;

ShaderProgram *ColoredTextureMat::_shader = NULL;

ColoredTextureMat::ColoredTextureMat(
    Texture *pDiffuseTexture, glm::vec3 pDiffuseColor):
    _diffuseColor(pDiffuseColor),
    _diffuseTexture(pDiffuseTexture),
    _prevLightCount(0),
    SpecularIntensity(10),
    SpecularPower(225)
{
    _lazyInitializeShader();
}

ColoredTextureMat::~ColoredTextureMat() {
    //dtor
}

void ColoredTextureMat::_lazyInitializeShader()
{
    if (_shader) return;

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

    glGenBuffers(1, &_pointLightBufferID);
    glBindBuffer(GL_UNIFORM_BUFFER, _pointLightBufferID);
    glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(float) * 8, _pointLightData.begin()._Ptr, GL_STATIC_DRAW);

    glGenBuffers(1, &_spotLightBufferID);
    glBindBuffer(GL_UNIFORM_BUFFER, _spotLightBufferID);
    glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(float) * 12, _spotLightData.begin()._Ptr, GL_STATIC_DRAW);
}

void ColoredTextureMat::setDiffuseColor(glm::vec3 pDiffuseColor) {
    _diffuseColor = pDiffuseColor;
}

void ColoredTextureMat::render(
    World *pWorld,
    Mesh *pMesh,
    const glm::mat4 &pModelMatrix,
    const glm::mat4 &pViewMatrix,
    const glm::mat4 &pProjectionMatrix
    )
{
    if (!_diffuseTexture) {
        printf("no texture-------------------------------------");
        _diffuseTexture = Texture::load(config::MGE_TEXTURE_PATH + "default.jpg");
        return;
    }

    _shader->use();
    _pointLightData.clear();
    _spotLightData.clear();

    if (pWorld->getLightCount() != 0) {
        glm::vec3 ambientClr = Light::GetAmbientColor();
        glm::vec3 viewerPos = pWorld->getMainCamera()->getWorldPosition();
        glUniform3f(_shader->getUniformLocation("ambientClr"), ambientClr.x, ambientClr.y, ambientClr.z);
        glUniform1f(_shader->getUniformLocation("ambientIntensity"), Light::GetAmbientIntensity());
        glUniform4f(_shader->getUniformLocation("viewerPosV"), viewerPos.x, viewerPos.y, viewerPos.z, 1);
        glUniform1f(_shader->getUniformLocation("specularIntensity"),SpecularIntensity);
        glUniform1f(_shader->getUniformLocation("specularPower"),SpecularPower);

        for (int i = 0; i < pWorld->getLightCount(); i++)
        {
            Light* light = pWorld->getLightAt(i);

            if (auto pPoint = dynamic_cast<PointLight*>(light)) {
                AddPointLight(pPoint);
            }
            else if (auto pDir = dynamic_cast<DirectionalLight*>(light)) {
                RenderDirectionalLight(pDir);
            }
            else if (auto pSpot = dynamic_cast<SpotLight*>(light)) {
                RenderSpotLight(pSpot);
            }
            else {
                // Unknown light type
            }
        }

        glUniform1i(_shader->getUniformLocation("lightCount"), _pointLightData.size() / 8);
        glUniform1i(_shader->getUniformLocation("spotLightCount"), _spotLightData.size() / 12);

        //is doing this every frame a good idea?
        glBufferSubData(GL_UNIFORM_BUFFER, 0, _pointLightData.size() * sizeof(float), _pointLightData.begin()._Ptr);
        //what is magic number 0?
        glBufferSubData(GL_UNIFORM_BUFFER, 0, _spotLightData.size() * sizeof(float), _spotLightData.begin()._Ptr);
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

void ColoredTextureMat::RenderSpotLight(const SpotLight *l)
{
    auto pos = l->getWorldPosition();
    //pos
    _spotLightData.push_back(pos.x);
    _spotLightData.push_back(pos.y);
    _spotLightData.push_back(pos.z);
    //cutoff
    _spotLightData.push_back(l->GetCutoffAngleRadians());

    //dir
    _spotLightData.push_back(l->GetDirection().x);
    _spotLightData.push_back(l->GetDirection().y);
    _spotLightData.push_back(l->GetDirection().z);

    //intensity
    _spotLightData.push_back(l->Intensity);

    //clr
    auto clr = l->Color;
    _spotLightData.push_back(clr.r);
    _spotLightData.push_back(clr.g);
    _spotLightData.push_back(clr.b);

    //attenuation
    _spotLightData.push_back(l->GetLinearAttenuation());
    //what is magic number 1?
    glBindBufferBase(GL_UNIFORM_BUFFER, 2, _spotLightBufferID);

}

void ColoredTextureMat::RenderDirectionalLight(const DirectionalLight *pDir) {
    glUniform3f(_shader->getUniformLocation("directionalLight.direction"),
        pDir->Direction.x,
        pDir->Direction.y,
        pDir->Direction.z);
    glUniform3f(_shader->getUniformLocation("directionalLight.color"),
        pDir->Color.x,
        pDir->Color.y,
        pDir->Color.z);
    glUniform1f(_shader->getUniformLocation("directionalLight.intensity"),
           pDir->Intensity);
}

void ColoredTextureMat::AddPointLight(PointLight *l) {
    auto pos = l->getWorldPosition();
    //if (pos.x==0)
    //std::cout << pos << std::endl;
    //pos
    _pointLightData.push_back(pos.x);
    _pointLightData.push_back(pos.y);
    _pointLightData.push_back(pos.z);
    //intensity
    _pointLightData.push_back(l->Intensity);
    auto clr = l->Color;
    //clr
    _pointLightData.push_back(clr.r);
    _pointLightData.push_back(clr.g);
    _pointLightData.push_back(clr.b);
    //attenuation
    _pointLightData.push_back(l->getAttenuation());
    glBindBufferBase(GL_UNIFORM_BUFFER, 1, _pointLightBufferID);
}
