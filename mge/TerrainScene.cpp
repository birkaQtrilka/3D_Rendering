#include <iostream>
#include <string>
#include <memory>

#include "../glm.hpp"
#include "core/Renderer.hpp" //
#include "core/Mesh.hpp" //
#include "core/World.hpp" //
#include "core/Texture.hpp" //
#include "core/Light.hpp" //
#include "core/Camera.hpp"  //
#include "core/GameObject.hpp" //
#include "materials/AbstractMaterial.hpp" //
#include "materials/ColorMaterial.hpp" //

#include "TerrainScene.h" //
#include "behaviours/RotatingBehaviour.hpp" //
#include "util/DebugHud.hpp" //
#include "config.hpp" //
#include "MGEDemo.hpp" //

#include "behaviours/Follow.h"
#include "MyClasses/DirectionalLight.h"
#include "MyClasses/PointLight.h"
#include "MyClasses/SinMove.h"
#include "MyClasses/Materials/ColoredTextureMat.h"
#include "MyClasses/Materials/TerrainMat.h"

//construct the game class into _window, _renderer and hud (other parts are initialized by build)
TerrainScene::TerrainScene() : AbstractGame ()
{
}

void TerrainScene::initialize() {
    //setup the core part
    AbstractGame::initialize();

}

//build the game _world
void TerrainScene::_initializeScene()
{
    Mesh* floorMesh = Mesh::load (config::MGE_MODEL_PATH+"plane20x20_2048tris_aligned_uvs.obj");
    Mesh* cubeMeshF = Mesh::load (config::MGE_MODEL_PATH+"cube_flat.obj");

    //MATERIALS
    ColoredTextureMat* defaultMat = new ColoredTextureMat (nullptr,
         glm::vec3(.1f,0.1f,0));

    auto heightMap = Texture::load (config::MGE_TEXTURE_PATH+"TerrainStuff/heightmap.png");
    auto splatMap = Texture::load (config::MGE_TEXTURE_PATH+"TerrainStuff/splatmap.png");
    auto diffuseText1 = Texture::load (config::MGE_TEXTURE_PATH+"TerrainStuff/diffuse1.jpg");
    auto diffuseText2 = Texture::load (config::MGE_TEXTURE_PATH+"TerrainStuff/diffuse2.jpg");
    auto diffuseText3 = Texture::load (config::MGE_TEXTURE_PATH+"TerrainStuff/diffuse3.jpg");
    auto diffuseText4 = Texture::load (config::MGE_TEXTURE_PATH+"TerrainStuff/diffuse4.jpg");

    TerrainMat* terrainMat = new TerrainMat(
        heightMap,
        splatMap,
        std::vector{diffuseText1, diffuseText2,diffuseText3,diffuseText4});

    ColorMaterial* whiteCol = new ColorMaterial (glm::vec3(1,.2f,0));

    DirectionalLight* dirLight = new DirectionalLight(
            "dirLight",
            glm::vec3(-3,1,5),
            glm::vec3(-2,1,0)
        );
    dirLight->scale(glm::vec3(.1f, .1, .1));
    dirLight->setMesh(cubeMeshF);
    dirLight->setMaterial(whiteCol);
    dirLight->Color = glm::vec3(1,1,1);
    dirLight->Intensity = 2.0f;
    _world->add(dirLight);

    GameObject* floor = new GameObject ("floor", glm::vec3(0,-1,0));
    floor->setBehaviour(new RotatingBehaviour());
    floor->scale(glm::vec3(2.5f,1,2.5f));
    floor->setMesh (floorMesh);
    floor->setMaterial(terrainMat);
    _world->add(floor);

    GameObject* cube = new GameObject ("cuube", glm::vec3(0,0,2));
    cube->setMesh (cubeMeshF);
    cube->scale(glm::vec3(0.1f));
    cube->setMaterial(defaultMat);
    defaultMat->SpecularPower = 1;
    defaultMat->SpecularIntensity = 2;
    _world->add(cube);

    Camera* camera = new Camera ("camera", glm::vec3(0,0,0));
    camera->setBehaviour (new Follow(30, cube, camera));
    _world->add(camera);
    camera->rotate(0, glm::vec3(1,0,0));
    _world->setMainCamera(camera);



}

void TerrainScene::_render() {
    _renderer->setClearColor(255,0,0);
    AbstractGame::_render();

}


TerrainScene::~TerrainScene()
{
}
