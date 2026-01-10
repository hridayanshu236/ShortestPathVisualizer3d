#include "Pathfinding.h"
#include <cmath>
#include <algorithm>

Pathfinding::Pathfinding(Grid* grid)
    : grid(grid)
    , state(IDLE)
    , algorithm(ALGORITHM_DIJKSTRA)
    , startX(-1), startZ(-1)
    , goalX(-1), goalZ(-1)
    , nodesExplored(0)
    , pathLength(0)
    , executionTime(0.0f)
    , stepsPerSecond(20.0f)
    , timeSinceLastStep(0.0f)
{
}

Pathfinding::~Pathfinding()
{
    CleanupNodes();
}

bool Pathfinding::StartDijkstra(int startX, int startZ, int goalX, int goalZ)
{
    Reset();

    this->startX = startX;
    this->startZ = startZ;
    this->goalX = goalX;
    this->goalZ = goalZ;
    this->algorithm = ALGORITHM_DIJKSTRA;

    // Initialize start node
    Node* startNode = new Node(startX, startZ);
    startNode->gCost = 0;
    startNode->fCost = 0;

    allNodes[GetKey(startX, startZ)] = startNode;
    openSet.push(startNode);

    state = RUNNING;
    startTime = std::chrono::high_resolution_clock::now();

    return true;
}

bool Pathfinding::StartAStar(int startX, int startZ, int goalX, int goalZ)
{
    Reset();

    this->startX = startX;
    this->startZ = startZ;
    this->goalX = goalX;
    this->goalZ = goalZ;
    this->algorithm = ALGORITHM_ASTAR;

    // Initialize start node
    Node* startNode = new Node(startX, startZ);
    startNode->gCost = 0;
    startNode->hCost = Heuristic(startX, startZ, goalX, goalZ);
    startNode->fCost = startNode->gCost + startNode->hCost;

    allNodes[GetKey(startX, startZ)] = startNode;
    openSet.push(startNode);

    state = RUNNING;
    startTime = std::chrono::high_resolution_clock::now();

    return true;
}

void Pathfinding::Update(float deltaTime)
{
    if (state != RUNNING)
        return;

    timeSinceLastStep += deltaTime;

    // Process steps based on speed
    float stepInterval = 1.0f / stepsPerSecond;

    while (timeSinceLastStep >= stepInterval && state == RUNNING)
    {
        timeSinceLastStep -= stepInterval;

        if (algorithm == ALGORITHM_DIJKSTRA)
            StepDijkstra();
        else
            StepAStar();
    }
}

void Pathfinding::StepDijkstra()
{
    if (openSet.empty())
    {
        state = NO_PATH_FOUND;
        auto endTime = std::chrono::high_resolution_clock::now();
        executionTime = std::chrono::duration<float>(endTime - startTime).count();
        return;
    }

    // Get node with lowest cost
    Node* current = openSet.top();
    openSet.pop();

    // Mark as visited
    TileState currentState = grid->GetTile(current->x, current->z);
    if (currentState != START && currentState != GOAL)
        grid->SetTile(current->x, current->z, VISITED);

    nodesExplored++;
    closedSet.push_back(current);

    // Check if reached goal
    if (current->x == goalX && current->z == goalZ)
    {
        ReconstructPath(current);
        state = COMPLETED;
        auto endTime = std::chrono::high_resolution_clock::now();
        executionTime = std::chrono::duration<float>(endTime - startTime).count();
        return;
    }

    // Explore neighbors
    std::vector<Node*> neighbors = GetNeighbors(current);

    for (Node* neighbor : neighbors)
    {
        // Check if already in closed set
        if (std::find(closedSet.begin(), closedSet.end(), neighbor) != closedSet.end())
            continue;

        float tentativeGCost = current->gCost + 1.0f;  // Assuming unit cost

        Node* existingNeighbor = GetNode(neighbor->x, neighbor->z);

        if (existingNeighbor == nullptr)
        {
            // New node
            neighbor->gCost = tentativeGCost;
            neighbor->fCost = neighbor->gCost;  // Dijkstra doesn't use heuristic
            neighbor->parent = current;

            allNodes[GetKey(neighbor->x, neighbor->z)] = neighbor;
            openSet.push(neighbor);
        }
        else if (tentativeGCost < existingNeighbor->gCost)
        {
            // Better path found
            existingNeighbor->gCost = tentativeGCost;
            existingNeighbor->fCost = existingNeighbor->gCost;
            existingNeighbor->parent = current;

            delete neighbor;  // We're using existing node
        }
        else
        {
            delete neighbor;  // Not needed
        }
    }
}

void Pathfinding::StepAStar()
{
    if (openSet.empty())
    {
        state = NO_PATH_FOUND;
        auto endTime = std::chrono::high_resolution_clock::now();
        executionTime = std::chrono::duration<float>(endTime - startTime).count();
        return;
    }

    // Get node with lowest fCost
    Node* current = openSet.top();
    openSet.pop();

    // Mark as visited
    TileState currentState = grid->GetTile(current->x, current->z);
    if (currentState != START && currentState != GOAL)
        grid->SetTile(current->x, current->z, VISITED);

    nodesExplored++;
    closedSet.push_back(current);

    // Check if reached goal
    if (current->x == goalX && current->z == goalZ)
    {
        ReconstructPath(current);
        state = COMPLETED;
        auto endTime = std::chrono::high_resolution_clock::now();
        executionTime = std::chrono::duration<float>(endTime - startTime).count();
        return;
    }

    // Explore neighbors
    std::vector<Node*> neighbors = GetNeighbors(current);

    for (Node* neighbor : neighbors)
    {
        // Check if already in closed set
        if (std::find(closedSet.begin(), closedSet.end(), neighbor) != closedSet.end())
            continue;

        float tentativeGCost = current->gCost + 1.0f;

        Node* existingNeighbor = GetNode(neighbor->x, neighbor->z);

        if (existingNeighbor == nullptr)
        {
            // New node
            neighbor->gCost = tentativeGCost;
            neighbor->hCost = Heuristic(neighbor->x, neighbor->z, goalX, goalZ);
            neighbor->fCost = neighbor->gCost + neighbor->hCost;
            neighbor->parent = current;

            allNodes[GetKey(neighbor->x, neighbor->z)] = neighbor;
            openSet.push(neighbor);
        }
        else if (tentativeGCost < existingNeighbor->gCost)
        {
            // Better path found
            existingNeighbor->gCost = tentativeGCost;
            existingNeighbor->hCost = Heuristic(existingNeighbor->x, existingNeighbor->z, goalX, goalZ);
            existingNeighbor->fCost = existingNeighbor->gCost + existingNeighbor->hCost;
            existingNeighbor->parent = current;

            delete neighbor;
        }
        else
        {
            delete neighbor;
        }
    }
}

void Pathfinding::ReconstructPath(Node* endNode)
{
    pathLength = 0;
    Node* current = endNode;

    while (current != nullptr)
    {
        TileState state = grid->GetTile(current->x, current->z);
        if (state != START && state != GOAL)
            grid->SetTile(current->x, current->z, PATH);

        pathLength++;
        current = current->parent;
    }
}

float Pathfinding::Heuristic(int x1, int z1, int x2, int z2)
{
    // Manhattan distance
    return static_cast<float>(abs(x1 - x2) + abs(z1 - z2));
}

std::vector<Node*> Pathfinding::GetNeighbors(Node* node)
{
    std::vector<Node*> neighbors;

    // 4-directional movement (up, down, left, right)
    int dx[] = { 0, 0, -1, 1 };
    int dz[] = { -1, 1, 0, 0 };

    for (int i = 0; i < 4; i++)
    {
        int newX = node->x + dx[i];
        int newZ = node->z + dz[i];

        // Check bounds
        if (newX < 0 || newX >= Grid::SIZE || newZ < 0 || newZ >= Grid::SIZE)
            continue;

        // Check if obstacle
        TileState state = grid->GetTile(newX, newZ);
        if (state == OBSTACLE)
            continue;

        neighbors.push_back(new Node(newX, newZ));
    }

    return neighbors;
}

Node* Pathfinding::GetNode(int x, int z)
{
    int key = GetKey(x, z);
    auto it = allNodes.find(key);
    if (it != allNodes.end())
        return it->second;
    return nullptr;
}

void Pathfinding::Pause()
{
    if (state == RUNNING)
        state = PAUSED;
}

void Pathfinding::Resume()
{
    if (state == PAUSED)
        state = RUNNING;
}

void Pathfinding::Stop()
{
    state = IDLE;
}

void Pathfinding::Reset()
{
    CleanupNodes();

    while (!openSet.empty())
        openSet.pop();

    closedSet.clear();

    nodesExplored = 0;
    pathLength = 0;
    executionTime = 0.0f;
    timeSinceLastStep = 0.0f;
    state = IDLE;

    // Clear visited and path tiles
    for (int x = 0; x < Grid::SIZE; x++)
    {
        for (int z = 0; z < Grid::SIZE; z++)
        {
            TileState tileState = grid->GetTile(x, z);
            if (tileState == VISITED || tileState == PATH)
                grid->SetTile(x, z, EMPTY);
        }
    }
}

void Pathfinding::SetSpeed(float stepsPerSecond)
{
    this->stepsPerSecond = stepsPerSecond;
}

void Pathfinding::CleanupNodes()
{
    for (auto& pair : allNodes)
    {
        delete pair.second;
    }
    allNodes.clear();
}