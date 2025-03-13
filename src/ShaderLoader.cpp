#include "ShaderLoader.h"
#include <fstream>
#include <sstream>
#include <glad/glad.h>

ShaderLoader::ShaderLoader() : programID(0), vertexShaderID(0), fragmentShaderID(0) {}

ShaderLoader::~ShaderLoader()
{
    deleteProgram();
}

std::string ShaderLoader::readShaderFile(const std::string &filePath)
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

void ShaderLoader::checkShaderCompilation(unsigned int shader, const std::string &type)
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

void ShaderLoader::checkProgramLinking(unsigned int program)
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

unsigned int ShaderLoader::compileShader(unsigned int type, const std::string &source)
{
    unsigned int shader = glCreateShader(type);
    const char *src = source.c_str();
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);
    checkShaderCompilation(shader, type == GL_VERTEX_SHADER ? "Vertex" : "Fragment");
    return shader;
}

unsigned int ShaderLoader::loadVertexShader(const std::string &filePath)
{
    std::string source = readShaderFile(filePath);
    vertexShaderID = compileShader(GL_VERTEX_SHADER, source);
    return vertexShaderID;
}

unsigned int ShaderLoader::loadFragmentShader(const std::string &filePath)
{
    std::string source = readShaderFile(filePath);
    fragmentShaderID = compileShader(GL_FRAGMENT_SHADER, source);
    return fragmentShaderID;
}

void ShaderLoader::createShaderProgram(const std::string &vertexPath, const std::string &fragmentPath)
{
    deleteProgram();

    loadVertexShader(vertexPath);
    loadFragmentShader(fragmentPath);

    programID = glCreateProgram();
    glAttachShader(programID, vertexShaderID);
    glAttachShader(programID, fragmentShaderID);
    glLinkProgram(programID);
    checkProgramLinking(programID);

    deleteShaders();
}

void ShaderLoader::use()
{
    if (programID != 0)
        glUseProgram(programID);
}

unsigned int ShaderLoader::getProgramID() const
{
    return programID;
}

void ShaderLoader::deleteShaders()
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
}

void ShaderLoader::deleteProgram()
{
    if (programID != 0)
    {
        glDeleteProgram(programID);
        programID = 0;
    }
}
