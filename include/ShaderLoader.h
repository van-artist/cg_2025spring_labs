#ifndef SHADER_LOADER_H
#define SHADER_LOADER_H

#include <string>
#include <iostream>

class ShaderLoader
{
private:
    unsigned int programID;
    unsigned int vertexShaderID;
    unsigned int fragmentShaderID;

    std::string readShaderFile(const std::string &filePath);
    void checkShaderCompilation(unsigned int shader, const std::string &type);
    void checkProgramLinking(unsigned int program);
    unsigned int compileShader(unsigned int type, const std::string &source);
    unsigned int loadVertexShader(const std::string &filePath);
    unsigned int loadFragmentShader(const std::string &filePath);

public:
    ShaderLoader();
    ~ShaderLoader();

    void createShaderProgram(const std::string &vertexPath, const std::string &fragmentPath);
    void use();
    void deleteShaders();
    void deleteProgram();
    unsigned int getProgramID() const;
};

#endif
