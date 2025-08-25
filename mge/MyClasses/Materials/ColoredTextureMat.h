//
// Created by Birka on 25/11/2024.
//

#ifndef COLOREDTEXTUREMAT_H
#define COLOREDTEXTUREMAT_H

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>
#include <memory>
#include "../../materials/AbstractMaterial.hpp"
class DirectionalLight;
class SpotLight;
class PointLight;
class ShaderProgram;
class Texture;

// Host-side PODs matching the shader UBO std140 layout (keep same ordering/alignment)
struct PointLightUBO {
    glm::vec4 posIntensity; // xyz pos, w intensity
    glm::vec4 colorAttenuation; // rgb color, w attenuation
};

struct SpotLightUBO {
    glm::vec4 position_cutoffAngle; // xyz pos, w cutoffCos
    glm::vec4 direction_intensity; // xyz dir, w intensity
    glm::vec4 color_attenuation; // rgb color, w attenuation
    glm::vec4 innerCutoff_padding; // r innerCutoff, gba padding
};

class ColoredTextureMat : public AbstractMaterial{
public:
    ColoredTextureMat(Texture* pDiffuseTexture, glm::vec3 pColor = glm::vec3(1,0,0));
    virtual ~ColoredTextureMat();

    virtual void render(World* pWorld, Mesh* pMesh, const glm::mat4& pModelMatrix, const glm::mat4& pViewMatrix, const glm::mat4& pProjectionMatrix) override;
    void AddPointLight(PointLight* pPointLight);
    void setDiffuseColor (glm::vec3 pDiffuseColor);
    void RenderSpotLight(const SpotLight *l);
    void RenderDirectionalLight(const DirectionalLight *dl);

    float SpecularIntensity;
    float SpecularPower;
private:
    //all the static properties are shared between instances of ColorMaterial
    //note that they are all PRIVATE, we do not expose this static info to the outside world
    int _prevLightCount;

    // Use shared_ptr for shared resources to make ownership explicit and safe
    static std::shared_ptr<ShaderProgram> _shader;
    static void _lazyInitializeShader();
    // Host side UBO arrays (structs) to replace manual float packing
    static std::vector<PointLightUBO> _pointLightData;
    static std::vector<SpotLightUBO> _spotLightData;
    //this one is unique per instance of color material
    glm::vec3 _diffuseColor;
    std::shared_ptr<Texture> _diffuseTexture;
    //in this example we cache all identifiers for uniforms & attributes
    static GLint _uMVPMatrix;
    static GLint _uDiffuseTexture;

    static GLint _aVertex ;
    static GLint _aNormal;
    static GLint _aUV ;

    static GLint _uMMatrix;
    static GLuint _pointLightBufferID;
    static GLuint _spotLightBufferID;
    static GLint _uDiffuseColor;
    // cached uniform locations for frequently used uniforms (avoids repeated lookups)
    static GLint _uAmbientClr;
    static GLint _uAmbientIntensity;
    static GLint _uViewerPosV;
    static GLint _uSpecularIntensity;
    static GLint _uSpecularPower;
    static GLint _uLightCount;
    static GLint _uSpotLightCount;
};


#endif //COLOREDTEXTUREMAT_H
