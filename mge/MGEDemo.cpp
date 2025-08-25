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

#include "behaviours/RotatingBehaviour.hpp" //
#include "behaviours/KeysBehaviour.hpp" //
#include "util/DebugHud.hpp" //
#include "config.hpp" //
#include "MGEDemo.hpp" //

#include "behaviours/Follow.h"
#include "behaviours/RotateDirection.h"
#include "MyClasses/DirectionalLight.h"
#include "MyClasses/PointLight.h"
#include "MyClasses/SinMove.h"
#include "MyClasses/Materials/ColoredTextureMat.h"
#include "MyClasses/SpotLight.h"
#include "MyClasses/Materials/TerrainMat.h"

//construct the game class into _window, _renderer and hud (other parts are initialized by build)
MGEDemo::MGEDemo():AbstractGame (),_hud(0),_materials()
{
}

void MGEDemo::initialize() {
    //setup the core part
    AbstractGame::initialize();

    //setup the custom part so we can display some text
	std::cout << "Initializing HUD" << std::endl;
	_hud = new DebugHud(*_window);
	std::cout << "HUD initialized." << std::endl << std::endl;
}

//build the game _world
void MGEDemo::_initializeScene()
{
    //Mesh* planeMeshDefault = Mesh::load (config::MGE_MODEL_PATH+"plane.obj");
    Mesh* cubeMeshF = Mesh::load (config::MGE_MODEL_PATH+"cube_flat.obj");
    Mesh* sphereMeshS = Mesh::load (config::MGE_MODEL_PATH+"sphere_smooth.obj");
    Mesh* bananaMesh = Mesh::load (config::MGE_MODEL_PATH+"banana.obj");
    Mesh* suzannaMesh = Mesh::load (config::MGE_MODEL_PATH+"suzanna_smooth.obj");
    Mesh* floorMesh = Mesh::load (config::MGE_MODEL_PATH+"plane20x20_2tris_aligned_uvs.obj");

    //MATERIALS
    // AbstractMaterial* brickMat = new ColoredTextureMat (Texture::load (config::MGE_TEXTURE_PATH+"bricks.jpg"),
    //     glm::vec3(.2,.2,0));
    ColoredTextureMat* bananaMat = new ColoredTextureMat (Texture::load (config::MGE_TEXTURE_PATH+"Banana skin texture.jpg"),
         glm::vec3(.1f,0.1f,0));
    ColoredTextureMat* defaultMat = new ColoredTextureMat (nullptr,
         glm::vec3(.1f,0.1f,0));
    ColorMaterial* redCol = new ColorMaterial (glm::vec3(1,1,0));
    ColorMaterial* whiteCol = new ColorMaterial (glm::vec3(1,.2f,0));

    ColoredTextureMat* runicLight = new ColoredTextureMat (
        Texture::load (config::MGE_TEXTURE_PATH+"runicfloor.png"),
        glm::vec3(0,0,0)
    );
    ColoredTextureMat* floorMat = new ColoredTextureMat (
        Texture::load (config::MGE_TEXTURE_PATH+"default.jpg"),
        glm::vec3(-0.4,-0.4,-0.4)
    );


    //add a spinning sphere
    GameObject* sphere = new GameObject ("sphere", glm::vec3(0,0,0));
    sphere->scale(glm::vec3(1,1,1));
    sphere->setMesh (sphereMeshS);
    sphere->setMaterial(runicLight);
    sphere->setBehaviour (std::make_unique<KeysBehaviour>(bananaMesh,bananaMat ,bananaMat,10,110));
    _world->add(sphere);

    GameObject* floor = new GameObject ("floor", glm::vec3(0,-2,0));
    floor->scale(glm::vec3(20,1,20));
    floor->setMesh (floorMesh);
    floor->setMaterial(floorMat);
    floorMat->SpecularPower = 1;
    floorMat->SpecularIntensity = 0;
    _world->add(floor);

    GameObject* suzanna = new GameObject ("sphere", glm::vec3(0,1,8));
    suzanna->scale(glm::vec3(1,1,1));
    suzanna->setMesh (suzannaMesh);
    suzanna->setMaterial(defaultMat);
    defaultMat->SpecularPower = 1024;
    defaultMat->SpecularIntensity = 5;
    _world->add(suzanna);

    Camera* camera = new Camera ("camera", glm::vec3(0,6,16));
    camera->setBehaviour (std::make_unique<Follow>(5, sphere, camera));
    _world->add(camera);
    _world->setMainCamera(camera);

    SpotLight* spot_light = new SpotLight(
        "spotLight",
        glm::vec3(10,2,1),
        glm::vec3(0,-1,0),
        2,
        0.91,
        0.81f);

    spot_light->Color = glm::vec3(1,1,0);
    spot_light->Intensity = 10;
    // spot_light->setBehaviour(std::make_unique<SinMove>(
    //     3,
    //     .5f,
    //     glm::vec3(0,1,0),
    //     glm::vec3(0,0,0)
    //     //glm::vec3(rand() % 10,rand() % 10,(float)(rand() % 10)),
    //     //glm::vec3(rand() % 10,rand() % 10,(float)(rand() % 10))
    //     ));
    spot_light->setBehaviour(std::make_unique<RotateDirection>(
        spot_light,
        1,
        .5
        ));

    _world->add(spot_light);

    PointLight* light = new PointLight("light", glm::vec3(0,1,10),2);
    light->scale(glm::vec3(.1f, .1, .1));
    light->setMesh(cubeMeshF);
    light->setMaterial(redCol);
    light->Color = redCol->GetDiffuseColor();
    light->Intensity = 2.0f;
    _world->add(light);

    PointLight* light2 = new PointLight("light2", glm::vec3(-3,1,5),1);
    light2->scale(glm::vec3(.1f, .1, .1));
    light2->setMesh(cubeMeshF);
    light2->setMaterial(whiteCol);
    light2->Color = whiteCol->GetDiffuseColor();
    light2->setBehaviour(std::make_unique<SinMove>(
        3,
        5.0f,
        glm::vec3(0,1,0),
        glm::vec3(10,0,0)
        //glm::vec3(rand() % 10,rand() % 10,(float)(rand() % 10)),
        //glm::vec3(rand() % 10,rand() % 10,(float)(rand() % 10))
        ));
    light2->Intensity = 6.0f;
    _world->add(light2);

    DirectionalLight* dirLight = new DirectionalLight(
        "dirLight",
        glm::vec3(-3,1,5),
        glm::vec3(0,1,0)
    );
    dirLight->scale(glm::vec3(.1f, .1, .1));
    dirLight->setMesh(cubeMeshF);
    dirLight->setMaterial(whiteCol);
    dirLight->Color = glm::vec3(1,1,1);
    dirLight->Intensity = 1.0f;
    _world->add(dirLight);

}

void MGEDemo::_render() {
    _renderer->setClearColor(255,0,0);
    AbstractGame::_render();
    _updateHud();
}

void MGEDemo::_updateHud() {
    std::string debugInfo = "";
    debugInfo += std::string ("FPS:") + std::to_string((int)_fps)+"\n";

    _hud->setDebugInfo(debugInfo);
    _hud->draw();
}

MGEDemo::~MGEDemo()
{
	for (int i = 0; i < _materials.size(); i++) {
	    delete _materials[i];
	}
}
