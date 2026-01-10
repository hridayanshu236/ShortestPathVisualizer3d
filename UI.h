#ifndef UI_H
#define UI_H

#include <string>
#include "H:\CE\III-II\III-II\COMP 342\Project\PathfindingVisualizer\libs\imgui\imgui-1.92.2b\imgui.h"
#include "H:\CE\III-II\III-II\COMP 342\Project\PathfindingVisualizer\libs\imgui\imgui-1.92.2b\backends\imgui_impl_glfw.h"
#include "H:\CE\III-II\III-II\COMP 342\Project\PathfindingVisualizer\libs\imgui\imgui-1.92.2b\backends\imgui_impl_opengl3.h"
#include "Grid.h"
#include "Pathfinding.h"  

class UI
{
public:
    UI();
    ~UI();

    bool Initialize(GLFWwindow* window);
    void Shutdown();

    void NewFrame();
    void Render();

    // Get current state
    EditMode GetCurrentMode() const { return currentMode; }
    bool ShouldClearGrid() const { return clearGridRequested; }
    bool ShouldRunDijkstra() const { return runDijkstraRequested; }
    bool ShouldRunAStar() const { return runAStarRequested; }
    bool ShouldPause() const { return pauseRequested; }
    bool ShouldResume() const { return resumeRequested; }
    bool ShouldStop() const { return stopRequested; }
    float GetSpeed() const { return speed; }

    // Reset request flags
    void ResetRequests();

    // Update status
    void SetStatus(const std::string& status);
    void SetGridStats(bool hasStart, bool hasGoal, int obstacleCount);
    void SetPathfindingState(PathfindingState state, AlgorithmType algorithm,
        int nodesExplored, int pathLength, float executionTime);

    // Minimap
    void SetGrid(Grid* grid) { this->grid = grid; }  

    glm::vec3 GetLightPosition() const { return lightPosition; }
    float GetAmbientStrength() const { return ambientStrength; }
    float GetSpecularStrength() const { return specularStrength; }
    float GetShininess() const { return shininess; }

private:
    void RenderMinimap(); 
    void RenderMinimapRectangular(); 
    void RenderMinimapCircular();     
    void RenderMinimapZoomed();      

    void RenderControlPanel();

    EditMode currentMode;
    bool clearGridRequested;
    bool runDijkstraRequested;
    bool runAStarRequested;
    bool pauseRequested;
    bool resumeRequested;
    bool stopRequested;
    glm::vec3 lightPosition;
    float ambientStrength;
    float specularStrength;
    float shininess;
    std::string statusText;
    bool gridHasStart;
    bool gridHasGoal;
    int obstacleCount;

    // Pathfinding stats
    PathfindingState pathfindingState;
    AlgorithmType currentAlgorithm;
    int nodesExplored;
    int pathLength;
    float executionTime;

    // Settings
    float speed;

    // Minimap
    Grid* grid; 
    float minimapSize; 
    bool showMinimap;  
    // NEW: Minimap clipping options
    enum MinimapShape {
        SHAPE_RECTANGULAR,
        SHAPE_CIRCULAR,
        SHAPE_ZOOMED
    };
    MinimapShape minimapShape;
    float zoomLevel;  // For zoomed view
    glm::vec2 zoomCenter;  // Center of zoom (grid coordinates)
    bool controlPanelMinimized;
};

#endif