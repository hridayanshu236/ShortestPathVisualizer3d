#include "Grid.h"

Grid::Grid()
    : hasStart(false)
    , hasGoal(false)
    , startX(-1)
    , startZ(-1)
    , goalX(-1)
    , goalZ(-1)
{
    ClearGrid();
}

TileState Grid::GetTile(int x, int z) const
{
    if (x < 0 || x >= SIZE || z < 0 || z >= SIZE)
        return EMPTY;
    return tiles[x][z];
}

void Grid::SetTile(int x, int z, TileState state)
{
    if (x < 0 || x >= SIZE || z < 0 || z >= SIZE)
        return;
    tiles[x][z] = state;
}

void Grid::ClearGrid()
{
    for (int x = 0; x < SIZE; x++)
        for (int z = 0; z < SIZE; z++)
            tiles[x][z] = EMPTY;

    hasStart = false;
    hasGoal = false;
}

glm::vec3 Grid::GetTileColor(int x, int z) const
{
    return GetTileColor(GetTile(x, z));
}

glm::vec3 Grid::GetTileColor(TileState state) const
{
    switch (state)
    {
    case EMPTY:    return glm::vec3(0.95f, 0.95f, 0.95f);  // White
    case START:    return glm::vec3(0.2f, 0.8f, 0.2f);     // Green
    case GOAL:      return glm::vec3(0.8f, 0.2f, 0.2f);     // Red
    case OBSTACLE: return glm::vec3(0.15f, 0.15f, 0.15f);  // Black
    case VISITED:  return glm::vec3(0.4f, 0.6f, 0.9f);     // Blue
    case PATH:     return glm::vec3(1.0f, 0.8f, 0.2f);     // Yellow
    default:       return glm::vec3(0.95f, 0.95f, 0.95f);
    }
}

void Grid::SetStart(int x, int z)
{
    if (hasStart)
        tiles[startX][startZ] = EMPTY;

    tiles[x][z] = START;
    startX = x;
    startZ = z;
    hasStart = true;
}

void Grid::SetGoal(int x, int z)
{
    if (hasGoal)
        tiles[goalX][goalZ] = EMPTY;

    tiles[x][z] = GOAL;
    goalX = x;
    goalZ = z;
    hasGoal = true;
}

void Grid::ClearStart()
{
    if (hasStart)
    {
        tiles[startX][startZ] = EMPTY;
        hasStart = false;
    }
}

void Grid::ClearGoal()
{
    if (hasGoal)
    {
        tiles[goalX][goalZ] = EMPTY;
        hasGoal = false;
    }
}

glm::vec3 Grid::GetTileWorldPosition(int x, int z) const
{
    float xPos = (float)x - SIZE / 2.0f + 0.5f;
    float zPos = (float)z - SIZE / 2.0f + 0.5f;
    return glm::vec3(xPos, 0.0f, zPos);
}

void Grid::GetTileBounds(int x, int z, glm::vec3& min, glm::vec3& max) const
{
    glm::vec3 center = GetTileWorldPosition(x, z);
    min = glm::vec3(center.x - 0.45f, 0.0f, center.z - 0.45f);
    max = glm::vec3(center.x + 0.45f, 0.15f, center.z + 0.45f);
}