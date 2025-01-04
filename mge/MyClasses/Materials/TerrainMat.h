//
// Created by Birka on 03/01/2025.
//

#ifndef TERRAINMAT_H
#define TERRAINMAT_H

#include <GL/glew.h>
#include <glm/fwd.hpp>
#include <vector>
#include "../../materials/AbstractMaterial.hpp"
class PointLight;
class ShaderProgram;
class Texture;

class TerrainMat : public AbstractMaterial
{
public:
    TerrainMat(Texture* pHeightMap,Texture* pSplatMap, std::vector<Texture*>&& pDiffuseTextures);
    virtual ~TerrainMat();

    float GetSpecularIntensity() const;
    void SetSpecularIntensity(float specularIntensity);
    float GetSpecularPower() const;
    void SetSpecularPower(float pSpecularPower);
private:
    float _specularIntensity;
    float _specularPower;
    float _maxHeight;
    Texture* _heightMapTexture;
    Texture* _splatMapTexture;
    std::vector<Texture*> _diffuseTextures;
    std::vector<GLint> _diffuseTextureUnifs;

    virtual void render(World* pWorld, Mesh* pMesh, const glm::mat4& pModelMatrix, const glm::mat4& pViewMatrix, const glm::mat4& pProjectionMatrix) override;

    static void _lazyInitializeShader();

    static GLuint _uMMatrix;
    static GLint _heightMapUnif;
    static GLuint _splatMapUnif;
    static ShaderProgram* _shader;
    static GLint _aVertex ;
    static GLint _aNormal;
    static GLint _aUV ;
    static GLint _uMVPMatrix ;
};



#endif //TERRAINMAT_H
