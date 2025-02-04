cmake_minimum_required(VERSION 3.26)
project(MGE_CLion)

set(CMAKE_CXX_STANDARD 17)
file(GLOB_RECURSE SOURCES
        mge/*.h
        mge/*.cpp
)

add_executable(MGE_CLion ${SOURCE_FILES} ${SOURCES}
        glm.hpp main.cpp
        mge/behaviours/Follow.cpp
        mge/behaviours/Follow.h
        mge/behaviours/Mover.cpp
        mge/behaviours/Mover.h
        mge/MyClasses/Materials/ColoredTextureMat.cpp
        mge/MyClasses/Materials/ColoredTextureMat.h
        mge/MyClasses/PointLight.cpp
        mge/MyClasses/PointLight.h
        mge/MyClasses/SinMove.cpp
        mge/MyClasses/SinMove.h
        mge/MyClasses/DirectionalLight.cpp
        mge/MyClasses/DirectionalLight.h
        mge/MyClasses/Materials/TerrainMat.cpp
        mge/MyClasses/Materials/TerrainMat.h
        mge/TerrainScene.cpp
        mge/TerrainScene.h
        mge/MyClasses/Materials/OceanMat.cpp
        mge/MyClasses/Materials/OceanMat.h
        mge/MyClasses/DayAndNight.cpp
        mge/MyClasses/DayAndNight.h)

find_package(SFML COMPONENTS system window graphics CONFIG REQUIRED)
target_link_libraries(MGE_CLion PRIVATE sfml-system sfml-network sfml-graphics sfml-window)
find_package(GLEW REQUIRED)
target_link_libraries(MGE_CLion PRIVATE GLEW::GLEW)