#include "Raycast.h"
#include <algorithm>
#include <cfloat>

glm::vec3 Raycast::GetMouseRay(double mouseX, double mouseY,
    int screenWidth, int screenHeight,
    const glm::mat4& view,
    const glm::mat4& projection)
{
    // Normalize to NDC
    float x = (2.0f * static_cast<float>(mouseX)) / screenWidth - 1.0f;
    float y = 1.0f - (2.0f * static_cast<float>(mouseY)) / screenHeight;
    // Clip space
    glm::vec4 rayClip = glm::vec4(x, y, -1.0f, 1.0f);

    // Eye space
    glm::vec4 rayEye = glm::inverse(projection) * rayClip;
    rayEye = glm::vec4(rayEye.x, rayEye.y, -1.0f, 0.0f);

    // World space
    glm::vec3 rayWorld = glm::vec3(glm::inverse(view) * rayEye);
    return glm::normalize(rayWorld);
}

bool Raycast::RayIntersectsTile(const glm::vec3& rayOrigin,
    const glm::vec3& rayDir,
    int gridX, int gridZ,
    float& distance)
{
    // Calculate tile bounds
    float tileX = (float)gridX - Grid::SIZE / 2.0f + 0.5f;
    float tileZ = (float)gridZ - Grid::SIZE / 2.0f + 0.5f;

    float minX = tileX - 0.45f, maxX = tileX + 0.45f;
    float minY = 0.0f, maxY = 0.15f;
    float minZ = tileZ - 0.45f, maxZ = tileZ + 0.45f;

    // Ray-AABB intersection
    float tMin = (minX - rayOrigin.x) / rayDir.x;
    float tMax = (maxX - rayOrigin.x) / rayDir.x;
    if (tMin > tMax) std::swap(tMin, tMax);

    float tyMin = (minY - rayOrigin.y) / rayDir.y;
    float tyMax = (maxY - rayOrigin.y) / rayDir.y;
    if (tyMin > tyMax) std::swap(tyMin, tyMax);

    if ((tMin > tyMax) || (tyMin > tMax)) return false;
    if (tyMin > tMin) tMin = tyMin;
    if (tyMax < tMax) tMax = tyMax;

    float tzMin = (minZ - rayOrigin.z) / rayDir.z;
    float tzMax = (maxZ - rayOrigin.z) / rayDir.z;
    if (tzMin > tzMax) std::swap(tzMin, tzMax);

    if ((tMin > tzMax) || (tzMin > tMax)) return false;
    if (tzMin > tMin) tMin = tzMin;

    if (tMin < 0) return false;

    distance = tMin;
    return true;
}

bool Raycast::GetClickedTile(double mouseX, double mouseY,
    int screenWidth, int screenHeight,
    const glm::vec3& cameraPos,
    const glm::mat4& view,
    const glm::mat4& projection,
    int& outX, int& outZ)
{
    glm::vec3 rayDir = GetMouseRay(mouseX, mouseY, screenWidth, screenHeight, view, projection);

    float closestDist = FLT_MAX;
    int selectedX = -1, selectedZ = -1;

    for (int x = 0; x < Grid::SIZE; x++)
    {
        for (int z = 0; z < Grid::SIZE; z++)
        {
            float dist;
            if (RayIntersectsTile(cameraPos, rayDir, x, z, dist))
            {
                if (dist < closestDist)
                {
                    closestDist = dist;
                    selectedX = x;
                    selectedZ = z;
                }
            }
        }
    }

    if (selectedX != -1 && selectedZ != -1)
    {
        outX = selectedX;
        outZ = selectedZ;
        return true;
    }

    return false;
}