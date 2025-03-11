#include "ShaderLoader.h"
#include <fstream>
#include <sstream>
#include <glad/glad.h>

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
    std::string shaderCode = buffer.str();

    if (shaderCode.empty())
    {
        std::cerr << "ERROR: Shader file is empty: " << filePath << std::endl;
    }
    else
    {
        std::cout << "Shader loaded successfully: " << filePath << std::endl;
    }

    return shaderCode;
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
    return compileShader(GL_VERTEX_SHADER, source);
}

unsigned int ShaderLoader::loadFragmentShader(const std::string &filePath)
{
    std::string source = readShaderFile(filePath);
    return compileShader(GL_FRAGMENT_SHADER, source);
}

unsigned int ShaderLoader::createShaderProgram(const std::string &vertexPath, const std::string &fragmentPath)
{
    unsigned int vertexShader = loadVertexShader(vertexPath);
    unsigned int fragmentShader = loadFragmentShader(fragmentPath);

    if (vertexShader == 0 || fragmentShader == 0)
    {
        std::cerr << "Shader compilation failed!" << std::endl;
        return 0;
    }

    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    checkProgramLinking(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}

void ShaderLoader::deleteShader(unsigned int shader)
{
    glDeleteShader(shader);
}

void ShaderLoader::deleteProgram(unsigned int program)
{
    glDeleteProgram(program);
}

ShaderLoader::ShaderLoader() {}

ShaderLoader::~ShaderLoader() {}
