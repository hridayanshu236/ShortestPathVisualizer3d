#pragma once
#ifndef GRID_H
#define GRID_H

#include <glm/glm.hpp>

// Tile states
enum TileState {
    EMPTY,
    START,
    GOAL,
    OBSTACLE,
    VISITED,
    PATH
};

// Edit modes
enum EditMode {
    MODE_START,
    MODE_GOAL,
    MODE_OBSTACLE,
    MODE_CLEAR
};

class Grid
{
public:
    static const int SIZE = 30;

    Grid();

    // Tile operations
    TileState GetTile(int x, int z) const;
    void SetTile(int x, int z, TileState state);
    void ClearGrid();

    // Get tile color based on state
    glm::vec3 GetTileColor(int x, int z) const;
    glm::vec3 GetTileColor(TileState state) const;

    // Start/Goal management
    void SetStart(int x, int z);
    void SetGoal(int x, int z);
    void ClearStart();
    void ClearGoal();

    bool HasStart() const { return hasStart; }
    bool HasGoal() const { return hasGoal; }
    void GetStart(int& x, int& z) const { x = startX; z = startZ; }
    void GetGoal(int& x, int& z) const { x = goalX; z = goalZ; }

    // World position helpers
    glm::vec3 GetTileWorldPosition(int x, int z) const;
    void GetTileBounds(int x, int z, glm::vec3& min, glm::vec3& max) const;

private:
    TileState tiles[SIZE][SIZE];

    bool hasStart;
    bool hasGoal;
    int startX, startZ;
    int goalX, goalZ;
};

#endif