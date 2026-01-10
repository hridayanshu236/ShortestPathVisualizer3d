#ifndef PATHFINDING_H
#define PATHFINDING_H

#include <vector>
#include <queue>
#include <unordered_map>
#include <chrono>
#include "Grid.h"

// Node for pathfinding
struct Node
{
    int x, z;
    float gCost;
    float hCost;
    float fCost;
    Node* parent;

    Node(int x, int z) : x(x), z(z), gCost(0), hCost(0), fCost(0), parent(nullptr) {}
};

// Custom comparator for priority queue (min-heap based on fCost)
struct NodeComparator
{
    bool operator()(const Node* a, const Node* b) const
    {
        return a->fCost > b->fCost;  // Greater than for min-heap
    }
};

// Pathfinding state
enum PathfindingState {
    IDLE,
    RUNNING,
    PAUSED,
    COMPLETED,
    NO_PATH_FOUND
};

// Algorithm type
enum AlgorithmType {
    ALGORITHM_DIJKSTRA,
    ALGORITHM_ASTAR
};

class Pathfinding
{
public:
    Pathfinding(Grid* grid);
    ~Pathfinding();

    bool StartDijkstra(int startX, int startZ, int goalX, int goalZ);
    bool StartAStar(int startX, int startZ, int goalX, int goalZ);

    void Update(float deltaTime);

    void Pause();
    void Resume();
    void Stop();
    void Reset();

    void SetSpeed(float stepsPerSecond);

    PathfindingState GetState() const { return state; }
    AlgorithmType GetAlgorithm() const { return algorithm; }
    int GetNodesExplored() const { return nodesExplored; }
    int GetPathLength() const { return pathLength; }
    float GetExecutionTime() const { return executionTime; }
    bool IsRunning() const { return state == RUNNING; }

private:
    Grid* grid;
    PathfindingState state;
    AlgorithmType algorithm;

    // Algorithm state - FIXED COMPARATOR! 
    std::priority_queue<Node*, std::vector<Node*>, NodeComparator> openSet;
    std::unordered_map<int, Node*> allNodes;
    std::vector<Node*> closedSet;

    int startX, startZ;
    int goalX, goalZ;

    int nodesExplored;
    int pathLength;
    float executionTime;
    std::chrono::high_resolution_clock::time_point startTime;

    float stepsPerSecond;
    float timeSinceLastStep;

    void StepDijkstra();
    void StepAStar();
    void ReconstructPath(Node* endNode);
    float Heuristic(int x1, int z1, int x2, int z2);
    std::vector<Node*> GetNeighbors(Node* node);
    Node* GetNode(int x, int z);
    int GetKey(int x, int z) { return x * 1000 + z; }
    void CleanupNodes();
};

#endif