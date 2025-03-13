#ifndef SHADER_LOADER_H
#define SHADER_LOADER_H

#include <string>
#include <iostream>

class ShaderLoader
{
private:
    unsigned int programID;        // 着色器程序 ID
    unsigned int vertexShaderID;   // 顶点着色器 ID
    unsigned int fragmentShaderID; // 片段着色器 ID

    std::string readShaderFile(const std::string &filePath);
    void checkShaderCompilation(unsigned int shader, const std::string &type);
    void checkProgramLinking(unsigned int program);
    unsigned int compileShader(unsigned int type, const std::string &source);
    unsigned int loadVertexShader(const std::string &filePath);
    unsigned int loadFragmentShader(const std::string &filePath);

public:
    ShaderLoader();  // 构造函数
    ~ShaderLoader(); // 析构函数

    void createShaderProgram(const std::string &vertexPath, const std::string &fragmentPath);
    void use();                        // 绑定着色器程序
    void deleteShaders();              // 删除着色器
    void deleteProgram();              // 删除程序
    unsigned int getProgramID() const; // 获取 programID
};

#endif
