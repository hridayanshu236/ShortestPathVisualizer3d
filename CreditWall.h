#ifndef CREDITWALL_H
#define CREDITWALL_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Shader.h"
#include <vector>
#include <map>
#include <string>

class CreditWall {
private:
    std::vector<glm::vec3> letterPositions;
    std::vector<glm::vec3> letterColors;
    unsigned int cubeVAO, cubeVBO;
    bool visible;

    void CreateLetterPattern();
    void SetupCubeMesh();
    void AddLetter(char letter, int startX, int startY, glm::vec3 color);
    std::map<char, std::vector<std::pair<int, int>>> GetLetterPatterns();

public:
    CreditWall();
    ~CreditWall();

    void Render(Shader& shader);
    void Hide() { visible = false; }
    void Show() { visible = true; }
    bool IsVisible() const { return visible; }
};

#endif