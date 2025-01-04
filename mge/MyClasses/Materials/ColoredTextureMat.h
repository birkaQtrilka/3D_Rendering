//
// Created by Birka on 25/11/2024.
//

#ifndef COLOREDTEXTUREMAT_H
#define COLOREDTEXTUREMAT_H

#include <GL/glew.h>
#include <glm/fwd.hpp>
#include <vector>
#include "../../materials/AbstractMaterial.hpp"
class PointLight;
class ShaderProgram;
class Texture;

class ColoredTextureMat : public AbstractMaterial{
public:
    ColoredTextureMat(Texture* pDiffuseTexture, glm::vec3 pColor = glm::vec3(1,0,0));
    virtual ~ColoredTextureMat();

    virtual void render(World* pWorld, Mesh* pMesh, const glm::mat4& pModelMatrix, const glm::mat4& pViewMatrix, const glm::mat4& pProjectionMatrix) override;
    void AddPointLight(PointLight* pPointLight);
    void setDiffuseColor (glm::vec3 pDiffuseColor);
    float SpecularIntensity;
    float SpecularPower;
private:
    //all the static properties are shared between instances of ColorMaterial
    //note that they are all PRIVATE, we do not expose this static info to the outside world
    int _prevLightCount;

    void OnLigthAdded();
    static ShaderProgram* _shader;
    static void _lazyInitializeShader();

    static std::vector<float> _lightData;
    //this one is unique per instance of color material
    glm::vec3 _diffuseColor;
    Texture* _diffuseTexture;
    //in this example we cache all identifiers for uniforms & attributes
    static GLint _uMVPMatrix;
    static GLint _uDiffuseTexture;

    static GLint _aVertex ;
    static GLint _aNormal;
    static GLint _aUV ;

    static GLint _uMMatrix;
    static GLuint _lightBuffer;
};


#endif //COLOREDTEXTUREMAT_H
