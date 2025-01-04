//
// Created by Birka on 03/01/2025.
//

#ifndef TERRAINSCENE_H
#define TERRAINSCENE_H
#include "TerrainScene.h"

#include "core/AbstractGame.hpp"
#include "./materials/ColorMaterial.hpp" //
#include <vector>

class DebugHud;

class TerrainScene: public AbstractGame
{
    //PUBLIC FUNCTIONS

public:
    TerrainScene();
    virtual ~TerrainScene();

    //override initialize so we can add a DebugHud
    virtual void initialize();

protected:
    //override so we can construct the actual scene
    virtual void _initializeScene();

    //override render to render the hud as well.
    virtual void _render();

private:
    TerrainScene(const TerrainScene&);
    TerrainScene& operator=(const TerrainScene&);
};



#endif //TERRAINSCENE_H
