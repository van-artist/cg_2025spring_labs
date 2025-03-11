#pragma once
#include <iostream>
#include <string>

class ShaderLoader
{
private:
    static std::string readShaderFile(const std::string &filePath);
    static void checkShaderCompilation(unsigned int shader, const std::string &type);
    static void checkProgramLinking(unsigned int program);

public:
    ShaderLoader();
    ~ShaderLoader();

    static unsigned int loadVertexShader(const std::string &filePath);
    static unsigned int loadFragmentShader(const std::string &filePath);
    static unsigned int compileShader(unsigned int type, const std::string &source);

    static unsigned int createShaderProgram(const std::string &vertexPath, const std::string &fragmentPath);

    static void deleteShader(unsigned int shader);
    static void deleteProgram(unsigned int program);
};
