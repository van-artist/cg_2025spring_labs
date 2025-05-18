#include "Shader.h"
#include <fstream>
#include <sstream>
#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

Shader::Shader() : programID(0), vertexShaderID(0), fragmentShaderID(0), geometryShaderID(0) {}
Shader::Shader(const std::string &vertexPath, const std::string &fragmentPath, const std::string &geometryPath = "")
{
    createShaderProgram(vertexPath, fragmentPath, geometryPath);
}

Shader::~Shader()
{
    deleteProgram();
}

std::string Shader::readShaderFile(const std::string &filePath)
{
    std::ifstream file(filePath);
    if (!file.is_open())
    {
        std::cerr << "ERROR: Failed to open shader file: " << filePath << std::endl;
        return "";
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

void Shader::checkShaderCompilation(unsigned int shader, const std::string &type)
{
    int success;
    char infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        std::cerr << type << " shader compilation failed:\n"
                  << infoLog << std::endl;
    }
}

void Shader::checkProgramLinking(unsigned int program)
{
    int success;
    char infoLog[512];
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(program, 512, nullptr, infoLog);
        std::cerr << "Shader program linking failed:\n"
                  << infoLog << std::endl;
    }
}

unsigned int Shader::compileShader(unsigned int type, const std::string &source)
{
    unsigned int shader = glCreateShader(type);
    const char *src = source.c_str();
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);
    checkShaderCompilation(shader, type == GL_VERTEX_SHADER ? "Vertex" : (type == GL_FRAGMENT_SHADER ? "Fragment" : "Geometry"));
    return shader;
}

unsigned int Shader::loadVertexShader(const std::string &filePath)
{
    std::string source = readShaderFile(filePath);
    vertexShaderID = compileShader(GL_VERTEX_SHADER, source);
    return vertexShaderID;
}

unsigned int Shader::loadFragmentShader(const std::string &filePath)
{
    std::string source = readShaderFile(filePath);
    fragmentShaderID = compileShader(GL_FRAGMENT_SHADER, source);
    return fragmentShaderID;
}

unsigned int Shader::loadGeometryShader(const std::string &filePath)
{
    std::string source = readShaderFile(filePath);
    geometryShaderID = compileShader(GL_GEOMETRY_SHADER, source);
    return geometryShaderID;
}

void Shader::createShaderProgram(const std::string &vertexPath, const std::string &fragmentPath, const std::string &geometryPath)
{
    deleteProgram();

    loadVertexShader(vertexPath);
    loadFragmentShader(fragmentPath);

    programID = glCreateProgram();
    glAttachShader(programID, vertexShaderID);
    glAttachShader(programID, fragmentShaderID);
    if (!geometryPath.empty())
    {
        loadGeometryShader(geometryPath);
        glAttachShader(programID, geometryShaderID);
    }
    glLinkProgram(programID);
    checkProgramLinking(programID);

    deleteShaders();
}

void Shader::use()
{
    if (programID != 0)
        glUseProgram(programID);
}

unsigned int Shader::getProgramID() const
{
    return programID;
}

void Shader::deleteShaders()
{
    if (vertexShaderID != 0)
    {
        glDeleteShader(vertexShaderID);
        vertexShaderID = 0;
    }
    if (fragmentShaderID != 0)
    {
        glDeleteShader(fragmentShaderID);
        fragmentShaderID = 0;
    }
    if (geometryShaderID != 0)
    {
        glDeleteShader(geometryShaderID);
        geometryShaderID = 0;
    }
}

void Shader::deleteProgram()
{
    if (programID != 0)
    {
        glDeleteProgram(programID);
        programID = 0;
    }
}

void Shader::setInt(const std::string &name, int value)
{
    glUniform1i(glGetUniformLocation(programID, name.c_str()), value);
}

void Shader::setUniform1f(const std::string &name, float value)
{
    glUniform1f(glGetUniformLocation(programID, name.c_str()), value);
}

void Shader::setUniform3f(const std::string &name, const glm::vec3 &value)
{
    glUniform3f(glGetUniformLocation(programID, name.c_str()), value.x, value.y, value.z);
}

void Shader::setUniformMatrix4fv(const std::string &name, const glm::mat4 &matrix)
{
    glUniformMatrix4fv(glGetUniformLocation(programID, name.c_str()), 1, GL_FALSE, glm::value_ptr(matrix));
}

void Shader::setUniform3fv(const std::string &name, const float *values)
{
    glUniform3fv(glGetUniformLocation(programID, name.c_str()), 1, values);
}

void Shader::setUniform1i(const std::string &name, int value)
{
    int location = glGetUniformLocation(programID, name.c_str());
    if (location != -1)
    {
        glUniform1i(location, value);
    }
    else
    {
        std::cerr << "WARNING: Uniform " << name << " not found!" << std::endl;
    }
}

void Shader::setMat4(const std::string &name, const glm::mat4 &matrix) const
{
    glUniformMatrix4fv(glGetUniformLocation(programID, name.c_str()), 1, GL_FALSE, glm::value_ptr(matrix));
}