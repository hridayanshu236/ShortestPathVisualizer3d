#pragma once
#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>

class Shader
{
public:
    unsigned int ID;

    Shader(const char* vertexSource, const char* fragmentSource);
    ~Shader();

    void Use();
    void SetMat4(const std::string& name, const float* value);
    void SetVec3(const std::string& name, float x, float y, float z);
    void SetVec3(const std::string& name, const glm::vec3& value);
    void SetFloat(const std::string& name, float value); 

private:
    void CheckCompileErrors(unsigned int shader, std::string type);
};

namespace ShaderSource
{
    extern const char* VertexShader;
    extern const char* FragmentShader;
}

#endif