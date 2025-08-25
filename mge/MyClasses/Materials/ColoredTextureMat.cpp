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
#include <memory>
#include <algorithm>

GLint ColoredTextureMat::_uMVPMatrix = -1;
GLint ColoredTextureMat::_uMMatrix = -1;
GLint ColoredTextureMat::_uDiffuseTexture = -1;
GLint ColoredTextureMat::_aVertex = 0;
GLint ColoredTextureMat::_aNormal = 0;
GLint ColoredTextureMat::_aUV = 0;
std::shared_ptr<ShaderProgram> ColoredTextureMat::_shader = nullptr;
std::vector<PointLightUBO> ColoredTextureMat::_pointLightData;
std::vector<SpotLightUBO> ColoredTextureMat::_spotLightData;
GLuint ColoredTextureMat::_pointLightBufferID = 0;
GLuint ColoredTextureMat::_spotLightBufferID = 0;

GLint ColoredTextureMat::_uAmbientClr = -1;
GLint ColoredTextureMat::_uAmbientIntensity = -1;
GLint ColoredTextureMat::_uViewerPosV = -1;
GLint ColoredTextureMat::_uSpecularIntensity = -1;
GLint ColoredTextureMat::_uSpecularPower = -1;
GLint ColoredTextureMat::_uLightCount = -1;
GLint ColoredTextureMat::_uSpotLightCount = -1;
GLint ColoredTextureMat::_uDiffuseColor = -1;

constexpr int MAX_POINT_LIGHTS = 100;
constexpr int MAX_SPOT_LIGHTS = 100;

ColoredTextureMat::ColoredTextureMat(
    Texture *pDiffuseTexture, glm::vec3 pDiffuseColor):
    _diffuseColor(pDiffuseColor),
    _prevLightCount(0),
    _diffuseTexture([&]
    {
          Texture* tex = pDiffuseTexture ? pDiffuseTexture : Texture::load(config::MGE_TEXTURE_PATH + "default.jpg");
          return std::shared_ptr<Texture>(tex, [](Texture*){});
    }()),
    SpecularIntensity(10),
    SpecularPower(225)
{

    _lazyInitializeShader();
}

ColoredTextureMat::~ColoredTextureMat() {
    // Cleanup GL buffers owned by this material class (static buffers)
    if (_pointLightBufferID) { glDeleteBuffers(1, &_pointLightBufferID); _pointLightBufferID = 0; }
    if (_spotLightBufferID) { glDeleteBuffers(1, &_spotLightBufferID); _spotLightBufferID = 0; }
}

void ColoredTextureMat::_lazyInitializeShader()
{
    if (_shader) return;

    // Create and initialize shared shader program (owned by shared_ptr)
    _shader = std::make_shared<ShaderProgram>();
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

    // cache frequently used uniform locations
    _uAmbientClr = _shader->getUniformLocation("ambientClr");
    _uAmbientIntensity = _shader->getUniformLocation("ambientIntensity");
    _uViewerPosV = _shader->getUniformLocation("viewerPosV");
    _uSpecularIntensity = _shader->getUniformLocation("specularIntensity");
    _uSpecularPower = _shader->getUniformLocation("specularPower");
    _uLightCount = _shader->getUniformLocation("lightCount");
    _uSpotLightCount = _shader->getUniformLocation("spotLightCount");

    // Reserve host-side UBO arrays to avoid reallocations
    _pointLightData.reserve(MAX_POINT_LIGHTS);
    _spotLightData.reserve(MAX_SPOT_LIGHTS);

    // create UBO buffers
    glGenBuffers(1, &_pointLightBufferID);
    glBindBuffer(GL_UNIFORM_BUFFER, _pointLightBufferID);
    glBufferData(
        GL_UNIFORM_BUFFER,
        MAX_POINT_LIGHTS * sizeof(PointLightUBO),
        nullptr,
        GL_DYNAMIC_DRAW);

    glGenBuffers(1, &_spotLightBufferID);
    glBindBuffer(GL_UNIFORM_BUFFER, _spotLightBufferID);
    glBufferData(
        GL_UNIFORM_BUFFER,
        MAX_SPOT_LIGHTS * sizeof(SpotLightUBO),
        nullptr,
        GL_DYNAMIC_DRAW);

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
        return;
    }

    _shader->use();
    _pointLightData.clear();
    _spotLightData.clear();

    if (pWorld->getLightCount() != 0) {
        glm::vec3 ambientClr = Light::GetAmbientColor();
        glm::vec3 viewerPos = pWorld->getMainCamera()->getWorldPosition();
        if (_uAmbientClr != -1) glUniform3f(_uAmbientClr, ambientClr.x, ambientClr.y, ambientClr.z);
        if (_uAmbientIntensity != -1) glUniform1f(_uAmbientIntensity, Light::GetAmbientIntensity());
        if (_uViewerPosV != -1) glUniform4f(_uViewerPosV, viewerPos.x, viewerPos.y, viewerPos.z, 1);
        if (_uSpecularIntensity != -1) glUniform1f(_uSpecularIntensity, SpecularIntensity);
        if (_uSpecularPower != -1) glUniform1f(_uSpecularPower, SpecularPower);

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

        if (_uLightCount != -1) glUniform1i(_uLightCount, static_cast<GLint>(_pointLightData.size()));
        if (_uSpotLightCount != -1) glUniform1i(_uSpotLightCount, static_cast<GLint>(_spotLightData.size()));

        // Upload UBOs (clamped to max counts)
        size_t pointCount = std::min<size_t>(_pointLightData.size(), MAX_POINT_LIGHTS);
        size_t spotCount = std::min<size_t>(_spotLightData.size(), MAX_SPOT_LIGHTS);

        if (_pointLightBufferID) {
            glBindBufferBase(GL_UNIFORM_BUFFER, 1, _pointLightBufferID);
            glBufferSubData(GL_UNIFORM_BUFFER, 0, pointCount * sizeof(PointLightUBO), _pointLightData.data());
        }
        if (_spotLightBufferID) {
            glBindBufferBase(GL_UNIFORM_BUFFER, 2, _spotLightBufferID);
            glBufferSubData(GL_UNIFORM_BUFFER, 0, spotCount * sizeof(SpotLightUBO), _spotLightData.data());
        }
    }

    if (GLint loc = _uDiffuseColor; loc != -1) {
        glUniform3f(loc, _diffuseColor.r, _diffuseColor.g, _diffuseColor.b);
    } else if (GLint loc2 = _shader->getUniformLocation("diffuseColor"); loc2 != -1) {
        glUniform3f(loc2, _diffuseColor.r, _diffuseColor.g, _diffuseColor.b);
    }

    //setup texture slot 0
    glActiveTexture(GL_TEXTURE0);
    //bind the texture to the current active slot
    glBindTexture(GL_TEXTURE_2D, _diffuseTexture->getId());
    //tell the shader the texture slot for the diffuse texture is slot 0
    if (_uDiffuseTexture != -1) glUniform1i(_uDiffuseTexture, 0);

    //pass in a precalculate mvp matrix (see texture material for the opposite)
    glm::mat4 mvpMatrix = pProjectionMatrix * pViewMatrix * pModelMatrix;
    glUniformMatrix4fv(_uMVPMatrix, 1, GL_FALSE, glm::value_ptr(mvpMatrix));
    glUniformMatrix4fv(_uMMatrix, 1, GL_FALSE, glm::value_ptr(pModelMatrix));

    //now inform mesh of where to stream its data (use cached attribute locations)
    pMesh->streamToOpenGL(_aVertex, _aNormal, _aUV);
}

void ColoredTextureMat::RenderSpotLight(const SpotLight *l)
{
    auto pos = l->getWorldPosition();
    auto dir = l->GetDirection();
    auto clr = l->Color;
    SpotLightUBO entry;

    entry.position_cutoffAngle = glm::vec4(pos.x, pos.y, pos.z, glm::cos(l->GetCutoffAngleRadians()));
    entry.direction_intensity = glm::vec4(dir.x, dir.y, dir.z, l->Intensity);
    entry.color_attenuation = glm::vec4(clr.r, clr.g, clr.b, l->GetLinearAttenuation());
    entry.innerCutoff_padding = glm::vec4(l->GetInnerCutoffAngleRadians(), 0,0,0);
    _spotLightData.emplace_back(entry);
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
    PointLightUBO entry;
    auto pos = l->getWorldPosition();
    entry.posIntensity = glm::vec4(pos.x, pos.y, pos.z, l->Intensity);
    auto clr = l->Color;
    entry.colorAttenuation = glm::vec4(clr.r, clr.g, clr.b, l->getAttenuation());
    _pointLightData.emplace_back(entry);
}
