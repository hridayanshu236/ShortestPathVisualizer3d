#include "UI.h"
#include "SutherlandHodgman.h"  
#include <GLFW/glfw3.h>

UI::UI()
    : currentMode(MODE_START)
    , clearGridRequested(false)
    , runDijkstraRequested(false)
    , runAStarRequested(false)
    , pauseRequested(false)
    , resumeRequested(false)
    , stopRequested(false)
    , statusText("Ready")
    , gridHasStart(false)
    , gridHasGoal(false)
    , obstacleCount(0)
    , pathfindingState(IDLE)
    , currentAlgorithm(ALGORITHM_DIJKSTRA)
    , nodesExplored(0)
    , pathLength(0)
    , executionTime(0.0f)
    , speed(20.0f)
    , grid(nullptr)
    , minimapSize(200.0f)
    , showMinimap(true)
    , controlPanelMinimized(false)
    , lightPosition(5.0f, 10.0f, 5.0f)
    , ambientStrength(0.3f)
    , specularStrength(0.5f)
    , shininess(32.0f)
    , minimapShape(SHAPE_RECTANGULAR)
    , zoomLevel(1.0f)
    , zoomCenter(Grid::SIZE / 2.0f, Grid::SIZE / 2.0f)
{
}

UI::~UI()
{
}

bool UI::Initialize(GLFWwindow* window)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGui::StyleColorsDark();

    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowRounding = 5.0f;
    style.FrameRounding = 3.0f;
    style.GrabRounding = 3.0f;

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    return true;
}

void UI::Shutdown()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void UI::NewFrame()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

// ===== MINIMAP RENDERING METHODS ===== 

void UI::RenderMinimap()
{
    if (!showMinimap || grid == nullptr)
        return;

    // Choose rendering method based on shape
    switch (minimapShape)
    {
    case SHAPE_RECTANGULAR:
        RenderMinimapRectangular();
        break;
    case SHAPE_CIRCULAR:
        RenderMinimapCircular();
        break;
    case SHAPE_ZOOMED:
        RenderMinimapZoomed();
        break;
    }
}

void UI::RenderMinimapRectangular()
{
    ImGuiIO& io = ImGui::GetIO();
    float padding = 10.0f;

    ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x - minimapSize - padding, padding), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(minimapSize, minimapSize), ImGuiCond_Always);

    ImGui::Begin("Minimap", &showMinimap,
        ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoBackground);

    ImVec2 canvas_pos = ImGui::GetCursorScreenPos();
    ImVec2 canvas_size = ImVec2(minimapSize - 10, minimapSize - 10);
    ImDrawList* draw_list = ImGui::GetWindowDrawList();

    // Background
    draw_list->AddRectFilled(canvas_pos,
        ImVec2(canvas_pos.x + canvas_size.x, canvas_pos.y + canvas_size.y),
        IM_COL32(20, 20, 25, 230));

    // Draw tiles
    float tileSize = canvas_size.x / Grid::SIZE;

    for (int x = 0; x < Grid::SIZE; x++)
    {
        for (int z = 0; z < Grid::SIZE; z++)
        {
            TileState state = grid->GetTile(x, z);
            glm::vec3 color = grid->GetTileColor(state);

            ImU32 imColor = IM_COL32(
                (int)(color.r * 255),
                (int)(color.g * 255),
                (int)(color.b * 255),
                255
            );

            float px = canvas_pos.x + x * tileSize;
            float py = canvas_pos.y + z * tileSize;

            draw_list->AddRectFilled(
                ImVec2(px + 0.5f, py + 0.5f),
                ImVec2(px + tileSize - 0.5f, py + tileSize - 0.5f),
                imColor
            );
        }
    }

    // Grid lines
    for (int i = 0; i <= Grid::SIZE; i++)
    {
        float offset = i * tileSize;
        draw_list->AddLine(ImVec2(canvas_pos.x + offset, canvas_pos.y),
            ImVec2(canvas_pos.x + offset, canvas_pos.y + canvas_size.y),
            IM_COL32(40, 40, 50, 180), 1.0f);
        draw_list->AddLine(ImVec2(canvas_pos.x, canvas_pos.y + offset),
            ImVec2(canvas_pos.x + canvas_size.x, canvas_pos.y + offset),
            IM_COL32(40, 40, 50, 180), 1.0f);
    }

    // Border
    draw_list->AddRect(canvas_pos,
        ImVec2(canvas_pos.x + canvas_size.x, canvas_pos.y + canvas_size.y),
        IM_COL32(80, 80, 90, 255), 0.0f, 0, 2.0f);

    ImGui::End();
}

void UI::RenderMinimapCircular()
{
    ImGuiIO& io = ImGui::GetIO();
    float padding = 10.0f;

    ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x - minimapSize - padding, padding), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(minimapSize, minimapSize), ImGuiCond_Always);

    ImGui::Begin("Minimap", &showMinimap,
        ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoBackground);

    ImVec2 canvas_pos = ImGui::GetCursorScreenPos();
    ImVec2 canvas_size = ImVec2(minimapSize - 10, minimapSize - 10);
    ImDrawList* draw_list = ImGui::GetWindowDrawList();

    // Circle parameters
    glm::vec2 center(canvas_pos.x + canvas_size.x / 2, canvas_pos.y + canvas_size.y / 2);
    float radius = canvas_size.x / 2;

    // Draw circular background
    draw_list->AddCircleFilled(ImVec2(center.x, center.y), radius, IM_COL32(20, 20, 25, 230), 64);

    // ===== KEY FIX: Use circular clip rect ===== 
    // This automatically clips everything to the circle
    draw_list->PushClipRect(
        ImVec2(center.x - radius, center.y - radius),
        ImVec2(center.x + radius, center.y + radius),
        true
    );

    float tileSize = canvas_size.x / Grid::SIZE;

    // Draw tiles (will be auto-clipped)
    for (int x = 0; x < Grid::SIZE; x++)
    {
        for (int z = 0; z < Grid::SIZE; z++)
        {
            float px = canvas_pos.x + x * tileSize;
            float py = canvas_pos.y + z * tileSize;

            // Quick distance check to skip far tiles
            glm::vec2 tileCenter(px + tileSize / 2, py + tileSize / 2);
            float dist = glm::distance(tileCenter, center);
            if (dist > radius + tileSize)
                continue;

            TileState state = grid->GetTile(x, z);
            glm::vec3 color = grid->GetTileColor(state);

            ImU32 imColor = IM_COL32(
                (int)(color.r * 255),
                (int)(color.g * 255),
                (int)(color.b * 255),
                255
            );

            draw_list->AddRectFilled(
                ImVec2(px + 0.5f, py + 0.5f),
                ImVec2(px + tileSize - 0.5f, py + tileSize - 0.5f),
                imColor
            );
        }
    }

    // Draw grid lines (will be auto-clipped to circle)
    for (int i = 0; i <= Grid::SIZE; i++)
    {
        float offset = i * tileSize;

        // Vertical lines
        draw_list->AddLine(
            ImVec2(canvas_pos.x + offset, canvas_pos.y),
            ImVec2(canvas_pos.x + offset, canvas_pos.y + canvas_size.y),
            IM_COL32(40, 40, 50, 180),
            1.0f
        );

        // Horizontal lines
        draw_list->AddLine(
            ImVec2(canvas_pos.x, canvas_pos.y + offset),
            ImVec2(canvas_pos.x + canvas_size.x, canvas_pos.y + offset),
            IM_COL32(40, 40, 50, 180),
            1.0f
        );
    }

    // Pop clip rect
    draw_list->PopClipRect();

    // Draw circular border (outside clip region, so it's always full)
    draw_list->AddCircle(ImVec2(center.x, center.y), radius, IM_COL32(80, 80, 90, 255), 64, 3.0f);

    // Label
    draw_list->AddText(ImVec2(canvas_pos.x + 5, canvas_pos.y + 5),
        IM_COL32(255, 255, 255, 200), "Circular");

    ImGui::End();
}
void UI::RenderMinimapZoomed()
{
    ImGuiIO& io = ImGui::GetIO();
    float padding = 10.0f;

    ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x - minimapSize - padding, padding), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(minimapSize, minimapSize), ImGuiCond_Always);

    ImGui::Begin("Minimap", &showMinimap,
        ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoBackground);

    ImVec2 canvas_pos = ImGui::GetCursorScreenPos();
    ImVec2 canvas_size = ImVec2(minimapSize - 10, minimapSize - 10);
    ImDrawList* draw_list = ImGui::GetWindowDrawList();

    // Background
    draw_list->AddRectFilled(canvas_pos,
        ImVec2(canvas_pos.x + canvas_size.x, canvas_pos.y + canvas_size.y),
        IM_COL32(20, 20, 25, 230));

    // Calculate zoom window in grid coordinates
    float viewSize = Grid::SIZE / zoomLevel;

    float minX = zoomCenter.x - viewSize / 2;
    float maxX = zoomCenter.x + viewSize / 2;
    float minZ = zoomCenter.y - viewSize / 2;
    float maxZ = zoomCenter.y + viewSize / 2;

    // Clamp to grid bounds
    minX = glm::max(0.0f, minX);
    maxX = glm::min((float)Grid::SIZE, maxX);
    minZ = glm::max(0.0f, minZ);
    maxZ = glm::min((float)Grid::SIZE, maxZ);

    // Recalculate actual view size after clamping
    float actualViewSizeX = maxX - minX;
    float actualViewSizeZ = maxZ - minZ;
    float actualViewSize = glm::max(actualViewSizeX, actualViewSizeZ);

    float tileSize = canvas_size.x / actualViewSize;

    // Draw tiles
    int startX = (int)floor(minX);
    int endX = (int)ceil(maxX);
    int startZ = (int)floor(minZ);
    int endZ = (int)ceil(maxZ);

    startX = glm::max(0, startX);
    endX = glm::min(Grid::SIZE, endX);
    startZ = glm::max(0, startZ);
    endZ = glm::min(Grid::SIZE, endZ);

    for (int x = startX; x < endX; x++)
    {
        for (int z = startZ; z < endZ; z++)
        {
            TileState state = grid->GetTile(x, z);
            glm::vec3 color = grid->GetTileColor(state);

            ImU32 imColor = IM_COL32(
                (int)(color.r * 255),
                (int)(color.g * 255),
                (int)(color.b * 255),
                255
            );

            float px = canvas_pos.x + (x - minX) * tileSize;
            float py = canvas_pos.y + (z - minZ) * tileSize;

            draw_list->AddRectFilled(
                ImVec2(px + 0.5f, py + 0.5f),
                ImVec2(px + tileSize - 0.5f, py + tileSize - 0.5f),
                imColor
            );
        }
    }

    // Draw grid lines
    for (int i = startX; i <= endX; i++)
    {
        float x = canvas_pos.x + (i - minX) * tileSize;
        draw_list->AddLine(
            ImVec2(x, canvas_pos.y),
            ImVec2(x, canvas_pos.y + canvas_size.y),
            IM_COL32(40, 40, 50, 180),
            1.0f
        );
    }

    for (int i = startZ; i <= endZ; i++)
    {
        float y = canvas_pos.y + (i - minZ) * tileSize;
        draw_list->AddLine(
            ImVec2(canvas_pos.x, y),
            ImVec2(canvas_pos.x + canvas_size.x, y),
            IM_COL32(40, 40, 50, 180),
            1.0f
        );
    }

    // Border
    draw_list->AddRect(canvas_pos,
        ImVec2(canvas_pos.x + canvas_size.x, canvas_pos.y + canvas_size.y),
        IM_COL32(80, 80, 90, 255), 0.0f, 0, 2.0f);

    // ===== FIXED TEXT RENDERING - NO SPRINTF ===== 
    std::string zoomText = "Zoom: " + std::to_string(zoomLevel) + "x";
    draw_list->AddText(ImVec2(canvas_pos.x + 5, canvas_pos.y + 5),
        IM_COL32(255, 255, 255, 255), zoomText.c_str());

    ImGui::End();
}

void UI::Render()
{
    RenderControlPanel();
    RenderMinimap();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void UI::RenderControlPanel()
{
    if (controlPanelMinimized)
    {
        // ===== MINIMIZED VERSION ===== 
        ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(180, 60), ImGuiCond_Always);

        ImGui::Begin("Controls", nullptr,
            ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoScrollbar);

        ImGui::Text("Control Panel");
        ImGui::SameLine();

        if (ImGui::Button("[+] Show"))
        {
            controlPanelMinimized = false;
        }

        ImGui::End();
        return;
    }

    // ===== MAXIMIZED VERSION ===== 
    ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(300, 650), ImGuiCond_FirstUseEver);  // Increased height

    ImGui::Begin("Control Panel", nullptr, ImGuiWindowFlags_NoCollapse);

    // Minimize button
    ImGui::SameLine(ImGui::GetWindowWidth() - 90);
    if (ImGui::Button("[-] Hide"))
    {
        controlPanelMinimized = true;
    }

    ImGui::Separator();
    ImGui::Spacing();

    // ===== EDIT MODE SECTION ===== 
    ImGui::TextColored(ImVec4(0.4f, 0.8f, 1.0f, 1.0f), "Current Mode:");
    ImGui::SameLine();

    const char* modeName = "Unknown";
    ImVec4 modeColor = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);

    switch (currentMode)
    {
    case MODE_START:
        modeName = "SET START";
        modeColor = ImVec4(0.2f, 0.8f, 0.2f, 1.0f);
        break;
    case MODE_GOAL:
        modeName = "SET GOAL";
        modeColor = ImVec4(0.8f, 0.2f, 0.2f, 1.0f);
        break;
    case MODE_OBSTACLE:
        modeName = "ADD OBSTACLES";
        modeColor = ImVec4(0.3f, 0.3f, 0.3f, 1.0f);
        break;
    case MODE_CLEAR:
        modeName = "CLEAR TILE";
        modeColor = ImVec4(0.8f, 0.8f, 0.2f, 1.0f);
        break;
    }

    ImGui::TextColored(modeColor, "%s", modeName);

    ImGui::Separator();
    ImGui::Spacing();

    // Mode Selection Buttons
    ImGui::Text("Edit Mode:");

    if (ImGui::Button("Set Start Point", ImVec2(-1, 30)))
        currentMode = MODE_START;

    if (ImGui::Button("Set Goal Point", ImVec2(-1, 30)))
        currentMode = MODE_GOAL;

    if (ImGui::Button("Add Obstacles", ImVec2(-1, 30)))
        currentMode = MODE_OBSTACLE;

    if (ImGui::Button("Clear Tile", ImVec2(-1, 30)))
        currentMode = MODE_CLEAR;

    ImGui::Spacing();

    if (ImGui::Button("Clear Entire Grid", ImVec2(-1, 30)))
        clearGridRequested = true;

    ImGui::Separator();
    ImGui::Spacing();

    // ===== GRID STATUS ===== 
    ImGui::Text("Grid Status:");
    ImGui::BulletText("Start: %s", gridHasStart ? "Set" : "Not Set");
    ImGui::BulletText("Goal: %s", gridHasGoal ? "Set" : "Not Set");
    ImGui::BulletText("Obstacles: %d", obstacleCount);

    ImGui::Separator();
    ImGui::Spacing();

    // ===== ALGORITHM SECTION ===== 
    ImGui::Text("Pathfinding Algorithm:");

    bool canRun = gridHasStart && gridHasGoal &&
        (pathfindingState == IDLE ||
            pathfindingState == COMPLETED ||
            pathfindingState == NO_PATH_FOUND);

    bool isRunning = pathfindingState == RUNNING;
    bool isPaused = pathfindingState == PAUSED;

    if (!canRun)
        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.5f);

    if (ImGui::Button("Run Dijkstra", ImVec2(-1, 35)))
    {
        if (canRun)
            runDijkstraRequested = true;
    }

    if (ImGui::Button("Run A* Algorithm", ImVec2(-1, 35)))
    {
        if (canRun)
            runAStarRequested = true;
    }

    if (!canRun)
    {
        ImGui::PopStyleVar();

        if (!gridHasStart || !gridHasGoal)
        {
            ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.0f, 1.0f),
                "Set start and goal first!");
        }
        else if (isRunning)
        {
            ImGui::TextColored(ImVec4(0.2f, 1.0f, 0.2f, 1.0f),
                "Algorithm running.. .");
        }
    }

    // Control buttons during execution
    if (isRunning || isPaused)
    {
        ImGui::Spacing();

        if (isRunning && ImGui::Button("Pause", ImVec2(-1, 30)))
            pauseRequested = true;

        if (isPaused && ImGui::Button("Resume", ImVec2(-1, 30)))
            resumeRequested = true;

        if (ImGui::Button("Stop", ImVec2(-1, 30)))
            stopRequested = true;
    }

    ImGui::Spacing();

    // Speed control
    ImGui::Text("Animation Speed:");
    ImGui::SliderFloat("Steps/sec", &speed, 1.0f, 100.0f, "%.0f");

    ImGui::Separator();
    ImGui::Spacing();

    // ===== LIGHTING CONTROLS (GOURAUD) ===== 
    if (ImGui::CollapsingHeader("Lighting (Gouraud)"))
    {
        ImGui::Text("Light Position:");
        ImGui::SliderFloat("X##light", &lightPosition.x, -10.0f, 10.0f, "%.1f");
        ImGui::SliderFloat("Y##light", &lightPosition.y, 0.0f, 20.0f, "%.1f");
        ImGui::SliderFloat("Z##light", &lightPosition.z, -10.0f, 10.0f, "%.1f");

        ImGui::Spacing();
        ImGui::Text("Lighting Properties:");
        ImGui::SliderFloat("Ambient", &ambientStrength, 0.0f, 1.0f, "%. 2f");
        ImGui::SliderFloat("Specular", &specularStrength, 0.0f, 1.0f, "%.2f");
        ImGui::SliderFloat("Shininess", &shininess, 1.0f, 128.0f, "%.0f");

        ImGui::Spacing();
        if (ImGui::Button("Reset Lighting", ImVec2(-1, 25)))
        {
            lightPosition = glm::vec3(5.0f, 10.0f, 5.0f);
            ambientStrength = 0.3f;
            specularStrength = 0.5f;
            shininess = 32.0f;
        }

        ImGui::Spacing();
        ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "Method:  Gouraud Shading");
    }

    ImGui::Separator();
    ImGui::Spacing();

    // ===== MINIMAP OPTIONS (NEW!) ===== 
    if (ImGui::CollapsingHeader("Minimap Options"))
    {
        ImGui::Text("Shape (Sutherland-Hodgman):");

        if (ImGui::RadioButton("Rectangular", minimapShape == SHAPE_RECTANGULAR))
            minimapShape = SHAPE_RECTANGULAR;
        ImGui::SameLine();
        ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.0f), "(No clipping)");

        if (ImGui::RadioButton("Circular", minimapShape == SHAPE_CIRCULAR))
            minimapShape = SHAPE_CIRCULAR;
        ImGui::SameLine();
        ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.0f), "(Radar style)");

        if (ImGui::RadioButton("Zoomed", minimapShape == SHAPE_ZOOMED))
            minimapShape = SHAPE_ZOOMED;
        ImGui::SameLine();
        ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.0f), "(Detail view)");

        if (minimapShape == SHAPE_ZOOMED)
        {
            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Text("Zoom Controls:");
            ImGui::SliderFloat("Zoom Level", &zoomLevel, 1.0f, 5.0f, "%.1fx");
            ImGui::SliderFloat("Center X", &zoomCenter.x, 0.0f, (float)Grid::SIZE, "%.1f");
            ImGui::SliderFloat("Center Z", &zoomCenter.y, 0.0f, (float)Grid::SIZE, "%.1f");

            if (ImGui::Button("Reset Zoom", ImVec2(-1, 25)))
            {
                zoomLevel = 1.0f;
                zoomCenter = glm::vec2(Grid::SIZE / 2.0f, Grid::SIZE / 2.0f);
            }
        }

        ImGui::Spacing();
        ImGui::SliderFloat("Minimap Size", &minimapSize, 150.0f, 400.0f, "%.0f px");

        ImGui::Spacing();
        ImGui::TextColored(ImVec4(0.5f, 0.8f, 1.0f, 1.0f), "Clipping Algorithm:");
        ImGui::TextWrapped("Sutherland-Hodgman polygon clipping for circular and zoomed views");
    }

    ImGui::Separator();
    ImGui::Spacing();

    // ===== STATISTICS ===== 
    ImGui::TextColored(ImVec4(1.0f, 0.8f, 0.2f, 1.0f), "Statistics:");

    const char* algoName = (currentAlgorithm == ALGORITHM_DIJKSTRA) ? "Dijkstra" : "A*";
    ImGui::BulletText("Algorithm: %s", algoName);

    const char* stateName = "Idle";
    ImVec4 stateColor = ImVec4(0.7f, 0.7f, 0.7f, 1.0f);

    switch (pathfindingState)
    {
    case IDLE:
        stateName = "Idle";
        stateColor = ImVec4(0.7f, 0.7f, 0.7f, 1.0f);
        break;
    case RUNNING:
        stateName = "Running... ";
        stateColor = ImVec4(0.2f, 1.0f, 0.2f, 1.0f);
        break;
    case PAUSED:
        stateName = "Paused";
        stateColor = ImVec4(1.0f, 0.8f, 0.2f, 1.0f);
        break;
    case COMPLETED:
        stateName = "Path Found!";
        stateColor = ImVec4(0.2f, 0.8f, 1.0f, 1.0f);
        break;
    case NO_PATH_FOUND:
        stateName = "No Path Found";
        stateColor = ImVec4(1.0f, 0.2f, 0.2f, 1.0f);
        break;
    }

    ImGui::Text("Status:");
    ImGui::SameLine();
    ImGui::TextColored(stateColor, "%s", stateName);

    ImGui::BulletText("Nodes Explored: %d", nodesExplored);
    ImGui::BulletText("Path Length: %d", pathLength);
    ImGui::BulletText("Time: %. 3f sec", executionTime);

    ImGui::Separator();
    ImGui::Spacing();

    // Status message
    ImGui::TextColored(ImVec4(0.4f, 1.0f, 0.4f, 1.0f), "Info:");
    ImGui::TextWrapped("%s", statusText.c_str());

    ImGui::Spacing();
    ImGui::Separator();

    // Help
    ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "Controls:");
    ImGui::BulletText("TAB - Toggle cursor");
    ImGui::BulletText("W/A/S/D - Move camera");
    ImGui::BulletText("Scroll - Zoom");
    ImGui::BulletText("Click tiles to edit");

    ImGui::End();
}

void UI::ResetRequests()
{
    clearGridRequested = false;
    runDijkstraRequested = false;
    runAStarRequested = false;
    pauseRequested = false;
    resumeRequested = false;
    stopRequested = false;
}

void UI::SetStatus(const std::string& status)
{
    statusText = status;
}

void UI::SetGridStats(bool hasStart, bool hasGoal, int obstacleCount)
{
    gridHasStart = hasStart;
    gridHasGoal = hasGoal;
    this->obstacleCount = obstacleCount;
}

void UI::SetPathfindingState(PathfindingState state, AlgorithmType algorithm,
    int nodesExplored, int pathLength, float executionTime)
{
    this->pathfindingState = state;
    this->currentAlgorithm = algorithm;
    this->nodesExplored = nodesExplored;
    this->pathLength = pathLength;
    this->executionTime = executionTime;
}