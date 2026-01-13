#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "H:\CE\III-II\III-II\COMP 342\Project\PathfindingVisualizer\libs\imgui\imgui-1.92.2b\imgui.h" 
#include <iostream>
#include <vector>
#include "Pathfinding.h" 
#include "Camera.h"
#include "Grid.h"
#include "Shader.h"
#include "Raycast.h"
#include "CreditWall.h"
#include "UI.h"  

// Window settings
const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;

// Actual current window size (updates on resize)
int currentWidth = SCR_WIDTH;
int currentHeight = SCR_HEIGHT;

// Global objects
Camera camera;
Grid grid;
UI ui;
CreditWall* creditWall = nullptr;  // Pointer for proper initialization
Pathfinding pathfinding(&grid);

// Timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// Mouse state
bool cursorEnabled = true;
bool firstMouse = true;
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;

// Callbacks
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void processInput(GLFWwindow* window);

int main()
{
    // Startup message
    std::cout << "\n";
    std::cout << "================================================\n";
    std::cout << "   3D PATHFINDING VISUALIZER                   \n";
    std::cout << "                                               \n";
    std::cout << "   Developed with <3 by:                        \n";
    std::cout << "      * Hridayanshu                            \n";
    std::cout << "      * Sumira                                 \n";
    std::cout << "                                               \n";
    std::cout << "   Click any tile to begin!                     \n";
    std::cout << "================================================\n";
    std::cout << "\n";


    // Initialize GLFW
    if (!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4);

    // Create window
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "3D Pathfinding Visualizer", NULL, NULL);
    if (!window)
    {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    // Load GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;

    // Set callbacks
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    // Initialize UI
    if (!ui.Initialize(window))
    {
        std::cerr << "Failed to initialize UI" << std::endl;
        return -1;
    }
    ui.SetGrid(&grid);

    std::cout << "UI initialized successfully!" << std::endl;

    // OpenGL settings
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);

    // Create shader
    Shader shader(ShaderSource::VertexShader, ShaderSource::FragmentShader);

    // Create credit wall AFTER OpenGL context is ready
    creditWall = new CreditWall();
    std::cout << "Credit wall initialized!" << std::endl;

    // Create grid lines
    std::vector<float> gridVertices;
    for (int i = 0; i <= Grid::SIZE; i++)
    {
        float pos = (float)i - Grid::SIZE / 2.0f;
        gridVertices.push_back(-Grid::SIZE / 2.0f); gridVertices.push_back(0.16f); gridVertices.push_back(pos);
        gridVertices.push_back(Grid::SIZE / 2.0f); gridVertices.push_back(0.16f); gridVertices.push_back(pos);
        gridVertices.push_back(pos); gridVertices.push_back(0.16f); gridVertices.push_back(-Grid::SIZE / 2.0f);
        gridVertices.push_back(pos); gridVertices.push_back(0.16f); gridVertices.push_back(Grid::SIZE / 2.0f);
    }

    unsigned int gridVBO, gridVAO;
    glGenVertexArrays(1, &gridVAO);
    glGenBuffers(1, &gridVBO);
    glBindVertexArray(gridVAO);
    glBindBuffer(GL_ARRAY_BUFFER, gridVBO);
    glBufferData(GL_ARRAY_BUFFER, gridVertices.size() * sizeof(float), gridVertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Create tile geometry WITH NORMALS
    float tileVertices[] = {
        // Positions          // Normals
        // Top face (Y+)
        -0.45f, 0.15f, -0.45f,  0.0f, 1.0f, 0.0f,
         0.45f, 0.15f, -0.45f,  0.0f, 1.0f, 0.0f,
         0.45f, 0.15f,  0.45f,  0.0f, 1.0f, 0.0f,
         0.45f, 0.15f,  0.45f,  0.0f, 1.0f, 0.0f,
        -0.45f, 0.15f,  0.45f,  0.0f, 1.0f, 0.0f,
        -0.45f, 0.15f, -0.45f,  0.0f, 1.0f, 0.0f,

        // Bottom face (Y-)
        -0.45f, 0.0f, -0.45f,  0.0f, -1.0f, 0.0f,
         0.45f, 0.0f, -0.45f,  0.0f, -1.0f, 0.0f,
         0.45f, 0.0f,  0.45f,  0.0f, -1.0f, 0.0f,
         0.45f, 0.0f,  0.45f,  0.0f, -1.0f, 0.0f,
        -0.45f, 0.0f,  0.45f,  0.0f, -1.0f, 0.0f,
        -0.45f, 0.0f, -0.45f,  0.0f, -1.0f, 0.0f,

        // Front face (Z+)
        -0.45f, 0.0f,  0.45f,  0.0f, 0.0f, 1.0f,
         0.45f, 0.0f,  0.45f,  0.0f, 0.0f, 1.0f,
         0.45f, 0.15f,  0.45f,  0.0f, 0.0f, 1.0f,
         0.45f, 0.15f,  0.45f,  0.0f, 0.0f, 1.0f,
        -0.45f, 0.15f,  0.45f,  0.0f, 0.0f, 1.0f,
        -0.45f, 0.0f,  0.45f,  0.0f, 0.0f, 1.0f,

        // Back face (Z-)
        -0.45f, 0.0f, -0.45f,  0.0f, 0.0f, -1.0f,
         0.45f, 0.0f, -0.45f,  0.0f, 0.0f, -1.0f,
         0.45f, 0.15f, -0.45f,  0.0f, 0.0f, -1.0f,
         0.45f, 0.15f, -0.45f,  0.0f, 0.0f, -1.0f,
        -0.45f, 0.15f, -0.45f,  0.0f, 0.0f, -1.0f,
        -0.45f, 0.0f, -0.45f,  0.0f, 0.0f, -1.0f,

        // Left face (X-)
        -0.45f, 0.15f,  0.45f,  -1.0f, 0.0f, 0.0f,
        -0.45f, 0.15f, -0.45f,  -1.0f, 0.0f, 0.0f,
        -0.45f, 0.0f, -0.45f,  -1.0f, 0.0f, 0.0f,
        -0.45f, 0.0f, -0.45f,  -1.0f, 0.0f, 0.0f,
        -0.45f, 0.0f,  0.45f,  -1.0f, 0.0f, 0.0f,
        -0.45f, 0.15f,  0.45f,  -1.0f, 0.0f, 0.0f,

        // Right face (X+)
         0.45f, 0.15f,  0.45f,  1.0f, 0.0f, 0.0f,
         0.45f, 0.15f, -0.45f,  1.0f, 0.0f, 0.0f,
         0.45f, 0.0f, -0.45f,  1.0f, 0.0f, 0.0f,
         0.45f, 0.0f, -0.45f,  1.0f, 0.0f, 0.0f,
         0.45f, 0.0f,  0.45f,  1.0f, 0.0f, 0.0f,
         0.45f, 0.15f,  0.45f,  1.0f, 0.0f, 0.0f
    };

    unsigned int tileVBO, tileVAO;
    glGenVertexArrays(1, &tileVAO);
    glGenBuffers(1, &tileVBO);
    glBindVertexArray(tileVAO);
    glBindBuffer(GL_ARRAY_BUFFER, tileVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(tileVertices), tileVertices, GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    std::cout << "\n=== 3D Pathfinding Visualizer ===" << std::endl;
    std::cout << "Use the UI panel to control the application!" << std::endl;

    // Render loop
    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        // Update pathfinding animation
        pathfinding.Update(deltaTime);

        // Update speed from UI
        pathfinding.SetSpeed(ui.GetSpeed());

        // Handle UI requests
        if (ui.ShouldClearGrid())
        {
            grid.ClearGrid();
            pathfinding.Reset();
            ui.SetStatus("Grid cleared");
            ui.ResetRequests();
        }

        if (ui.ShouldRunDijkstra())
        {
            // Hide credit wall when algorithm starts
            if (creditWall && creditWall->IsVisible())
            {
                creditWall->Hide();
            }

            pathfinding.Reset();
            int sx, sz, gx, gz;
            grid.GetStart(sx, sz);
            grid.GetGoal(gx, gz);
            pathfinding.StartDijkstra(sx, sz, gx, gz);
            ui.SetStatus("Running Dijkstra...");
            ui.ResetRequests();
        }

        if (ui.ShouldRunAStar())
        {
            // Hide credit wall when algorithm starts
            if (creditWall && creditWall->IsVisible())
            {
                creditWall->Hide();
            }

            pathfinding.Reset();
            int sx, sz, gx, gz;
            grid.GetStart(sx, sz);
            grid.GetGoal(gx, gz);
            pathfinding.StartAStar(sx, sz, gx, gz);
            ui.SetStatus("Running A*...");
            ui.ResetRequests();
        }

        if (ui.ShouldPause())
        {
            pathfinding.Pause();
            ui.SetStatus("Paused");
            ui.ResetRequests();
        }

        if (ui.ShouldResume())
        {
            pathfinding.Resume();
            ui.SetStatus("Resumed");
            ui.ResetRequests();
        }

        if (ui.ShouldStop())
        {
            pathfinding.Stop();
            pathfinding.Reset();
            ui.SetStatus("Stopped");
            ui.ResetRequests();
        }

        // Update pathfinding stats in UI
        ui.SetPathfindingState(
            pathfinding.GetState(),
            pathfinding.GetAlgorithm(),
            pathfinding.GetNodesExplored(),
            pathfinding.GetPathLength(),
            pathfinding.GetExecutionTime()
        );

        // Check if completed
        if (pathfinding.GetState() == COMPLETED)
        {
            ui.SetStatus("Path found successfully!");
        }
        else if (pathfinding.GetState() == NO_PATH_FOUND)
        {
            ui.SetStatus("No path exists!");
        }

        // Update UI stats
        int obstacleCount = 0;
        for (int x = 0; x < Grid::SIZE; x++)
            for (int z = 0; z < Grid::SIZE; z++)
                if (grid.GetTile(x, z) == OBSTACLE)
                    obstacleCount++;

        ui.SetGridStats(grid.HasStart(), grid.HasGoal(), obstacleCount);

        // Render
        glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.Use();

        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom),
            (float)currentWidth / (float)currentHeight,
            0.1f, 100.0f);

        shader.SetMat4("view", glm::value_ptr(view));
        shader.SetMat4("projection", glm::value_ptr(projection));

        // Set Gouraud lighting uniforms
        shader.SetVec3("lightPos", ui.GetLightPosition());
        shader.SetVec3("viewPos", camera.Position);
        shader.SetVec3("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));
        shader.SetFloat("ambientStrength", ui.GetAmbientStrength());
        shader.SetFloat("specularStrength", ui.GetSpecularStrength());
        shader.SetFloat("shininess", ui.GetShininess());

        // Render credit wall FIRST (behind everything)
        if (creditWall && creditWall->IsVisible())
        {
            creditWall->Render(shader);
        }

        // Draw tiles
        glBindVertexArray(tileVAO);
        for (int x = 0; x < Grid::SIZE; x++)
        {
            for (int z = 0; z < Grid::SIZE; z++)
            {
                glm::mat4 model = glm::mat4(1.0f);
                model = glm::translate(model, grid.GetTileWorldPosition(x, z));
                shader.SetMat4("model", glm::value_ptr(model));

                glm::vec3 color = grid.GetTileColor(x, z);
                shader.SetVec3("tileColor", color.r, color.g, color.b);

                glDrawArrays(GL_TRIANGLES, 0, 36);
            }
        }

        // Draw grid lines
        glBindVertexArray(gridVAO);
        glm::mat4 gridModel = glm::mat4(1.0f);
        shader.SetMat4("model", glm::value_ptr(gridModel));
        shader.SetVec3("tileColor", 0.15f, 0.15f, 0.15f);
        glDrawArrays(GL_LINES, 0, static_cast<GLsizei>(gridVertices.size() / 3));

        // Render UI (on top of everything)
        ui.NewFrame();
        ui.Render();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup
    delete creditWall;
    ui.Shutdown();
    glDeleteVertexArrays(1, &tileVAO);
    glDeleteBuffers(1, &tileVBO);
    glDeleteVertexArrays(1, &gridVAO);
    glDeleteBuffers(1, &gridVBO);

    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    // Toggle cursor with TAB
    static bool keyTab = false;
    if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS && !keyTab)
    {
        keyTab = true;
        cursorEnabled = !cursorEnabled;
        glfwSetInputMode(window, GLFW_CURSOR, cursorEnabled ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
        firstMouse = true;
    }
    if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_RELEASE) keyTab = false;

    // Camera movement
    if (!cursorEnabled)
    {
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) camera.ProcessKeyboard(FORWARD, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) camera.ProcessKeyboard(BACKWARD, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) camera.ProcessKeyboard(LEFT, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) camera.ProcessKeyboard(RIGHT, deltaTime);
    }
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
    currentWidth = width;
    currentHeight = height;
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (cursorEnabled) return;

    if (firstMouse)
    {
        lastX = static_cast<float>(xpos);
        lastY = static_cast<float>(ypos);
        firstMouse = false;
    }

    float xoffset = static_cast<float>(xpos) - lastX;
    float yoffset = lastY - static_cast<float>(ypos);
    lastX = static_cast<float>(xpos);
    lastY = static_cast<float>(ypos);

    camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    ImGuiIO& io = ImGui::GetIO();
    if (io.WantCaptureMouse)
        return;

    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    ImGuiIO& io = ImGui::GetIO();
    if (io.WantCaptureMouse)
        return;

    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS && cursorEnabled)
    {
        // Hide credit wall on first click
        if (creditWall && creditWall->IsVisible())
        {
            creditWall->Hide();
        }

        double mouseX, mouseY;
        glfwGetCursorPos(window, &mouseX, &mouseY);

        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom),
            (float)currentWidth / (float)currentHeight,
            0.1f, 100.0f);

        int x, z;
        if (Raycast::GetClickedTile(mouseX, mouseY, currentWidth, currentHeight,
            camera.Position, view, projection, x, z))
        {
            EditMode mode = ui.GetCurrentMode();

            if (mode == MODE_START)
            {
                grid.SetStart(x, z);
                ui.SetStatus("Start point set");
            }
            else if (mode == MODE_GOAL)
            {
                grid.SetGoal(x, z);
                ui.SetStatus("Goal point set");
            }
            else if (mode == MODE_OBSTACLE)
            {
                if (grid.GetTile(x, z) != START && grid.GetTile(x, z) != GOAL)
                {
                    grid.SetTile(x, z, OBSTACLE);
                    ui.SetStatus("Obstacle placed");
                }
            }
            else if (mode == MODE_CLEAR)
            {
                if (grid.GetTile(x, z) == START) grid.ClearStart();
                if (grid.GetTile(x, z) == GOAL) grid.ClearGoal();
                grid.SetTile(x, z, EMPTY);
                ui.SetStatus("Tile cleared");
            }
        }
    }
}