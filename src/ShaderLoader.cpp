#include "ShaderLoader.h"
#include <fstream>
#include <sstream>
#include <glad/glad.h>

ShaderLoader::ShaderLoader() : programID(0), vertexShaderID(0), fragmentShaderID(0) {}

ShaderLoader::~ShaderLoader()
{
    deleteProgram();
}

// 读取 Shader 文件
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

// 检查 Shader 编译
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

// 检查 Shader 程序链接
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

// 编译 Shader
unsigned int ShaderLoader::compileShader(unsigned int type, const std::string &source)
{
    unsigned int shader = glCreateShader(type);
    const char *src = source.c_str();
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);
    checkShaderCompilation(shader, type == GL_VERTEX_SHADER ? "Vertex" : "Fragment");
    return shader;
}

// 加载顶点着色器
unsigned int ShaderLoader::loadVertexShader(const std::string &filePath)
{
    std::string source = readShaderFile(filePath);
    vertexShaderID = compileShader(GL_VERTEX_SHADER, source);
    return vertexShaderID;
}

// 加载片段着色器
unsigned int ShaderLoader::loadFragmentShader(const std::string &filePath)
{
    std::string source = readShaderFile(filePath);
    fragmentShaderID = compileShader(GL_FRAGMENT_SHADER, source);
    return fragmentShaderID;
}

// **创建着色器程序**
void ShaderLoader::createShaderProgram(const std::string &vertexPath, const std::string &fragmentPath)
{
    // 先删除旧的 Shader
    deleteProgram();

    // 加载 & 编译 Shader
    loadVertexShader(vertexPath);
    loadFragmentShader(fragmentPath);

    // 创建 Shader 程序
    programID = glCreateProgram();
    glAttachShader(programID, vertexShaderID);
    glAttachShader(programID, fragmentShaderID);
    glLinkProgram(programID);
    checkProgramLinking(programID);

    // 删除着色器对象（已链接到 programID，不需要保留）
    deleteShaders();
}

// 绑定着色器
void ShaderLoader::use()
{
    if (programID != 0)
        glUseProgram(programID);
}

// 获取着色器程序 ID
unsigned int ShaderLoader::getProgramID() const
{
    return programID;
}

// 删除 Shader（仅删除单个 shader）
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

// 删除 Shader 程序
void ShaderLoader::deleteProgram()
{
    if (programID != 0)
    {
        glDeleteProgram(programID);
        programID = 0;
    }
}
