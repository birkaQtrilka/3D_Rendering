//
// Created by Birka on 03/01/2025.
//

#include "TerrainMat.h"
#include <GL/glew.h>

#include <utility>
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

GLint TerrainMat::_aVertex = 0;
GLint TerrainMat::_aNormal = 0;
GLint TerrainMat::_aUV = 0;
GLint TerrainMat::_uMVPMatrix = 0;
GLuint TerrainMat::_uMMatrix = 0;
GLint TerrainMat::_heightMapUnif = 0;
GLuint TerrainMat::_splatMapUnif = 0;
ShaderProgram *TerrainMat::_shader = NULL;


TerrainMat::TerrainMat(Texture *pHeightMap,Texture *pSplatMap, std::vector<Texture*>&& pDiffuseTextures) :
    _specularIntensity(0), _specularPower(1), _maxHeight(5.0f), _heightMapTexture(pHeightMap),
    _diffuseTextures(std::move(pDiffuseTextures)), _splatMapTexture(pSplatMap)
{
    _lazyInitializeShader();
    for (int i = 0; i < _diffuseTextures.size(); i++) {
        _diffuseTextureUnifs.push_back(_shader->getUniformLocation("diffuseTexture" + std::to_string(i)));
    }
}

TerrainMat::~TerrainMat() = default;

float TerrainMat::GetSpecularIntensity() const {
    return _specularIntensity;
}

void TerrainMat::SetSpecularIntensity(const float pSpecularIntensity) {
    _specularIntensity = pSpecularIntensity;
}

float TerrainMat::GetSpecularPower() const {
    return _specularPower;
}

void TerrainMat::SetSpecularPower(const float pSpecularPower) {
    _specularPower = pSpecularPower;
}

void TerrainMat::_lazyInitializeShader() {
    if (!_shader) {
        _shader = new ShaderProgram();
        _shader->addShader(GL_VERTEX_SHADER, config::MGE_SHADER_PATH + "terrain.vs");
        _shader->addShader(GL_FRAGMENT_SHADER, config::MGE_SHADER_PATH + "terrain.fs");
        _shader->finalize();

        _uMVPMatrix = _shader->getUniformLocation("mvpMatrix");
        _uMMatrix = _shader->getUniformLocation("mMatrix");
        _heightMapUnif = _shader->getUniformLocation("heightMap");
        _splatMapUnif = _shader->getUniformLocation("splatMap");

        _aVertex = _shader->getAttribLocation("vertex");
        _aNormal = _shader->getAttribLocation("normal");
        _aUV = _shader->getAttribLocation("uv");

        //glGenBuffers(1, &_lightBuffer);
        //glBindBuffer(GL_UNIFORM_BUFFER, _lightBuffer);
        //glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(float) * 8, _lightData.begin()._Ptr, GL_STATIC_DRAW);
    }
}


void TerrainMat::render(World *pWorld, Mesh *pMesh, const glm::mat4 &pModelMatrix, const glm::mat4 &pViewMatrix,
                               const glm::mat4 &pProjectionMatrix) {
    if (!_heightMapTexture) {
        printf("no texture-------------------------------------");
        _heightMapTexture = Texture::load(config::MGE_TEXTURE_PATH + "default.jpg");
        return;
    }

    _shader->use();
    
    if (pWorld->getLightCount() != 0) {
        Light& light = pWorld->getLightAt(0);

        glm::vec3 ambientClr = Light::GetAmbientColor();
        glm::vec3 viewerPos = pWorld->getMainCamera().getWorldPosition();
        glUniform3f(_shader->getUniformLocation("ambientClr"), ambientClr.x, ambientClr.y, ambientClr.z);
        glUniform1f(_shader->getUniformLocation("ambientIntensity"), Light::GetAmbientIntensity());
        glUniform4f(_shader->getUniformLocation("viewerPosV"), viewerPos.x, viewerPos.y, viewerPos.z, 1);
        glUniform1f(_shader->getUniformLocation("specularIntensity"),_specularIntensity);
        glUniform1f(_shader->getUniformLocation("specularPower"),_specularPower);

        auto dirLight = dynamic_cast<DirectionalLight*>(&light);
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
    //setup texture slot 0
    glActiveTexture(GL_TEXTURE0);
    //bind the texture to the current active slot
    glBindTexture(GL_TEXTURE_2D, _heightMapTexture->getId());
    //tell the shader the texture slot for the diffuse texture is slot 0
    glUniform1i(_heightMapUnif, 0);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, _splatMapTexture->getId());
    glUniform1i(_splatMapUnif, 1);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, _diffuseTextures[0]->getId());
    glUniform1i(_diffuseTextureUnifs[0], 2);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, _diffuseTextures[1]->getId());
    glUniform1i(_diffuseTextureUnifs[1], 3);
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, _diffuseTextures[2]->getId());
    glUniform1i(_diffuseTextureUnifs[2], 4);
    glActiveTexture(GL_TEXTURE5);
    glBindTexture(GL_TEXTURE_2D, _diffuseTextures[3]->getId());
    glUniform1i(_diffuseTextureUnifs[3], 5);

    //pass in a precalculate mvp matrix (see texture material for the opposite)
    glm::mat4 mvpMatrix = pProjectionMatrix * pViewMatrix * pModelMatrix;
    glUniformMatrix4fv(_uMVPMatrix, 1, GL_FALSE, glm::value_ptr(mvpMatrix));
    glUniform1f(_shader->getUniformLocation("maxHeight"), _maxHeight);

    glUniformMatrix4fv(_uMMatrix, 1, GL_FALSE, glm::value_ptr(pModelMatrix));

    //now inform mesh of where to stream its data
    pMesh->streamToOpenGL(
        _shader->getAttribLocation("vertex"),
        _shader->getAttribLocation("normal"),
        _shader->getAttribLocation("uv")
    );
}