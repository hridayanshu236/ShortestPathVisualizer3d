#ifndef RAYCAST_H
#define RAYCAST_H

#include <glm/glm.hpp>
#include "Grid.h"

class Raycast
{
public:
    // Convert mouse position to world ray
    static glm::vec3 GetMouseRay(double mouseX, double mouseY,
        int screenWidth, int screenHeight,
        const glm::mat4& view,
        const glm::mat4& projection);

    // Test ray intersection with tile
    static bool RayIntersectsTile(const glm::vec3& rayOrigin,
        const glm::vec3& rayDir,
        int gridX, int gridZ,
        float& distance);

    // Find which tile was clicked
    static bool GetClickedTile(double mouseX, double mouseY,
        int screenWidth, int screenHeight,
        const glm::vec3& cameraPos,
        const glm::mat4& view,
        const glm::mat4& projection,
        int& outX, int& outZ);
};

#endif
