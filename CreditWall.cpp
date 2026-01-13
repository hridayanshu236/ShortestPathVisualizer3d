#include "CreditWall.h"
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

CreditWall::CreditWall() : visible(true)
{
    SetupCubeMesh();
    CreateLetterPattern();
    std::cout << "CreditWall created with " << letterPositions.size() << " pixels" << std::endl;
}

CreditWall:: ~CreditWall()
{
    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteBuffers(1, &cubeVBO);
}

void CreditWall::SetupCubeMesh()
{
    // Simple cube (0. 4 x 0.4 x 0.4)
    float size = 0.2f;
    float vertices[] = {
        // Front face
        -size, -size,  size,
         size, -size,  size,
         size,  size,  size,
         size,  size,  size,
        -size,  size,  size,
        -size, -size,  size,

        // Back face
        -size, -size, -size,
         size, -size, -size,
         size,  size, -size,
         size,  size, -size,
        -size,  size, -size,
        -size, -size, -size,

        // Top face
        -size,  size, -size,
         size,  size, -size,
         size,  size,  size,
         size,  size,  size,
        -size,  size,  size,
        -size,  size, -size,

        // Bottom face
        -size, -size, -size,
         size, -size, -size,
         size, -size,  size,
         size, -size,  size,
        -size, -size,  size,
        -size, -size, -size,

        // Left face
        -size, -size, -size,
        -size, -size,  size,
        -size,  size,  size,
        -size,  size,  size,
        -size,  size, -size,
        -size, -size, -size,

        // Right face
         size, -size, -size,
         size, -size,  size,
         size,  size,  size,
         size,  size,  size,
         size,  size, -size,
         size, -size, -size
    };

    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);

    glBindVertexArray(cubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
}

std::map<char, std::vector<std::pair<int, int>>> CreditWall::GetLetterPatterns()
{
    // 5x5 pixel font patterns (x, y coordinates)
    return {
        {'A', {{1,0},{0,1},{2,1},{0,2},{1,2},{2,2},{0,3},{2,3},{0,4},{2,4}}},
        {'B', {{0,0},{1,0},{0,1},{2,1},{0,2},{1,2},{0,3},{2,3},{0,4},{1,4}}},
        {'C', {{1,0},{2,0},{0,1},{0,2},{0,3},{1,4},{2,4}}},
        {'D', {{0,0},{1,0},{0,1},{2,1},{0,2},{2,2},{0,3},{2,3},{0,4},{1,4}}},
        {'E', {{0,0},{1,0},{2,0},{0,1},{0,2},{1,2},{0,3},{0,4},{1,4},{2,4}}},
        {'F', {{0,0},{1,0},{2,0},{0,1},{0,2},{1,2},{0,3},{0,4}}},
        {'G', {{1,0},{2,0},{0,1},{0,2},{1,2},{2,2},{0,3},{2,3},{1,4},{2,4}}},
        {'H', {{0,0},{2,0},{0,1},{2,1},{0,2},{1,2},{2,2},{0,3},{2,3},{0,4},{2,4}}},
        {'I', {{0,0},{1,0},{2,0},{1,1},{1,2},{1,3},{0,4},{1,4},{2,4}}},
        {'J', {{2,0},{2,1},{2,2},{0,3},{2,3},{1,4}}},
        {'K', {{0,0},{2,0},{0,1},{1,1},{0,2},{0,3},{1,3},{0,4},{2,4}}},
        {'L', {{0,0},{0,1},{0,2},{0,3},{0,4},{1,4},{2,4}}},
        {'M', {{0,0},{1,0},{2,0},{0,1},{2,1},{0,2},{2,2},{0,3},{2,3},{0,4},{2,4}}},
        {'N', {{0,0},{2,0},{0,1},{1,1},{2,1},{0,2},{2,2},{0,3},{2,3},{0,4},{2,4}}},
        {'O', {{1,0},{0,1},{2,1},{0,2},{2,2},{0,3},{2,3},{1,4}}},
        {'P', {{0,0},{1,0},{2,0},{0,1},{2,1},{0,2},{1,2},{0,3},{0,4}}},
        {'Q', {{1,0},{0,1},{2,1},{0,2},{2,2},{0,3},{2,3},{1,4},{2,4}}},
        {'R', {{0,0},{1,0},{0,1},{2,1},{0,2},{1,2},{0,3},{2,3},{0,4},{2,4}}},
        {'S', {{1,0},{2,0},{0,1},{1,2},{2,3},{0,4},{1,4}}},
        {'T', {{0,0},{1,0},{2,0},{1,1},{1,2},{1,3},{1,4}}},
        {'U', {{0,0},{2,0},{0,1},{2,1},{0,2},{2,2},{0,3},{2,3},{1,4}}},
        {'V', {{0,0},{2,0},{0,1},{2,1},{0,2},{2,2},{1,3},{1,4}}},
        {'W', {{0,0},{2,0},{0,1},{2,1},{0,2},{1,2},{2,2},{0,3},{2,3},{0,4},{2,4}}},
        {'X', {{0,0},{2,0},{0,1},{2,1},{1,2},{0,3},{2,3},{0,4},{2,4}}},
        {'Y', {{0,0},{2,0},{0,1},{2,1},{1,2},{1,3},{1,4}}},
        {'Z', {{0,0},{1,0},{2,0},{2,1},{1,2},{0,3},{0,4},{1,4},{2,4}}},
        {'0', {{1,0},{0,1},{2,1},{0,2},{2,2},{0,3},{2,3},{1,4}}},
        {'1', {{1,0},{1,1},{1,2},{1,3},{1,4}}},
        {'2', {{0,0},{1,0},{2,0},{2,1},{1,2},{0,3},{0,4},{1,4},{2,4}}},
        {'3', {{0,0},{1,0},{2,0},{2,1},{1,2},{2,3},{0,4},{1,4},{2,4}}},
        {'4', {{0,0},{2,0},{0,1},{2,1},{0,2},{1,2},{2,2},{2,3},{2,4}}},
        {'5', {{0,0},{1,0},{2,0},{0,1},{0,2},{1,2},{2,3},{0,4},{1,4}}},
        {'6', {{1,0},{2,0},{0,1},{0,2},{1,2},{0,3},{2,3},{1,4}}},
        {'7', {{0,0},{1,0},{2,0},{2,1},{1,2},{1,3},{1,4}}},
        {'8', {{1,0},{0,1},{2,1},{1,2},{0,3},{2,3},{1,4}}},
        {'9', {{1,0},{0,1},{2,1},{1,2},{2,2},{2,3},{1,4}}},
        {'&', {{1,0},{0,1},{1,2},{0,3},{2,3},{1,4},{2,4}}},
        {' ', {}}
    };
}

void CreditWall::CreateLetterPattern()
{
    letterPositions.clear();
    letterColors.clear();

    // Position behind and ABOVE the grid
    // Grid is 30x30, centered at origin
    // Grid tiles are at y = 0 to y = 0.15

    float wallZ = -8.0f;    // Behind the grid
    float wallY = 18.0f;    // HIGH above grid (was 10.0f - FIXED!)
    float scale = 0.5f;     // Letter size scale

    // Colors
    glm::vec3 titleColor(1.0f, 0.84f, 0.0f);      // Gold
    glm::vec3 subtitleColor(0.7f, 0.7f, 0.7f);    // Gray
    glm::vec3 nameColor1(0.4f, 0.7f, 1.0f);       // Blue (Hridayanshu)
    glm::vec3 nameColor2(1.0f, 0.5f, 0.7f);       // Pink (Sumira)
    glm::vec3 heartColor(1.0f, 0.2f, 0.3f);       // Red (&)

    // Line 1: "SHORTEST PATH FINDING"
    int x = -45;
    int y = 0;

    const char* line1 = "SHORTEST PATH FINDING";
    for (const char* c = line1; *c != '\0'; c++) {
        AddLetter(*c, x, y, titleColor);
        x += 6;
    }

    // Line 2: "VISUALIZER IN 3D"
    x = -35;
    y = 8;

    const char* line2 = "VISUALIZER IN 3D";
    for (const char* c = line2; *c != '\0'; c++) {
        AddLetter(*c, x, y, titleColor);
        x += 6;
    }

    // Line 3: "DEVELOPED BY"
    x = -30;
    y = 18;

    const char* line3 = "DEVELOPED BY";
    for (const char* c = line3; *c != '\0'; c++) {
        AddLetter(*c, x, y, subtitleColor);
        x += 6;
    }

    // Line 4: "HRIDAYANSHU & SUMIRA"
    x = -45;
    y = 28;

    const char* name1 = "HRIDAYANSHU";
    for (const char* c = name1; *c != '\0'; c++) {
        AddLetter(*c, x, y, nameColor1);
        x += 6;
    }

    x += 3;  // Space before &
    AddLetter('&', x, y, heartColor);
    x += 9;  // Space after &

    const char* name2 = "SUMIRA";
    for (const char* c = name2; *c != '\0'; c++) {
        AddLetter(*c, x, y, nameColor2);
        x += 6;
    }

    // Convert pixel coordinates to world positions
    for (size_t i = 0; i < letterPositions.size(); i++) {
        letterPositions[i].x = letterPositions[i].x * scale;
        letterPositions[i].y = wallY - letterPositions[i].y * scale;  // Top to bottom
        letterPositions[i].z = wallZ;
    }
}

void CreditWall::AddLetter(char letter, int startX, int startY, glm::vec3 color)
{
    auto patterns = GetLetterPatterns();

    // Check if pattern exists for this letter
    if (patterns.find(letter) == patterns.end()) {
        return;  // Skip unknown characters
    }

    // Add each pixel of the letter
    for (const auto& pixel : patterns[letter]) {
        letterPositions.push_back(glm::vec3(
            startX + pixel.first,   // x offset
            startY + pixel.second,  // y offset
            0                       // z (will be set later)
        ));
        letterColors.push_back(color);
    }
}

void CreditWall::Render(Shader& shader)
{
    if (!visible) return;

    glBindVertexArray(cubeVAO);

    // Render each letter pixel as a small cube
    for (size_t i = 0; i < letterPositions.size(); i++) {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, letterPositions[i]);

        shader.SetMat4("model", glm::value_ptr(model));
        shader.SetVec3("tileColor", letterColors[i]);

        glDrawArrays(GL_TRIANGLES, 0, 36);
    }

    glBindVertexArray(0);
}