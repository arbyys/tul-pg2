#include <iostream>

#include "App.hpp"

float App::GetMapY(float x, float z) {
    //  ^   3-----2
    //  |   |    /|
    //  |   |  /  |
    //  |   |/    |
    //  z   0-----1
    //   x ------->
    x = x - MAP_MOVE;
    z = z - MAP_MOVE;

    unsigned int step = 10 * MAP_SCALE; //step used in Model::CreateTerrain
    double InverseStep = 2.5; // 1/step

    unsigned int lowX = std::floor(x);
    lowX = lowX - lowX % step;
    unsigned int highX = lowX + step;

    unsigned int lowZ = std::floor(z);
    lowZ = lowZ - lowZ % step;
    unsigned int highZ = lowZ + step;

    float relativeX = (x - lowX)/step;
    float relativeZ = (z - lowZ)/step;

    float p3 = heights[{lowX * InverseStep, highZ * InverseStep}] * MAP_SCALE;
    float p2 = heights[{highX * InverseStep, highZ * InverseStep}] * MAP_SCALE;
    float p1 = heights[{highX * InverseStep, lowZ * InverseStep}] * MAP_SCALE;
    float p0 = heights[{lowX * InverseStep, lowZ * InverseStep}] * MAP_SCALE;
    float X3to2 = p3 * (1 - relativeX) + p2 * relativeX;
    float X0to1 = p0 * (1 - relativeX) + p1 * relativeX;
    
    float finalHeight = X0to1 * (1 - relativeZ) + X3to2 * relativeZ;
    return finalHeight;
}