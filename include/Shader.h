#ifndef SHADER_LOADER_H
#define SHADER_LOADER_H

#include <string>
#include <iostream>
#include <glm/glm.hpp>

class Shader
{
private:
    unsigned int programID;
    unsigned int vertexShaderID;
    unsigned int fragmentShaderID;
    unsigned int geometryShaderID;

    std::string readShaderFile(const std::string &filePath);
    void checkShaderCompilation(unsigned int shader, const std::string &type);
    void checkProgramLinking(unsigned int program);
    unsigned int compileShader(unsigned int type, const std::string &source);
    unsigned int loadVertexShader(const std::string &filePath);
    unsigned int loadFragmentShader(const std::string &filePath);
    unsigned int loadGeometryShader(const std::string &filePath);

public:
    Shader();
    Shader(const std::string &vertexPath, const std::string &fragmentPath, const std::string &geometryPath = "");
    ~Shader();

    void createShaderProgram(const std::string &vertexPath, const std::string &fragmentPath, const std::string &geometryPath = "");
    void use();
    void deleteShaders();
    void deleteProgram();
    unsigned int getProgramID() const;

    void setInt(const std::string &name, int value);
    void setUniform1i(const std::string &name, int value);
    void setUniform1f(const std::string &name, float value);
    void setUniform3f(const std::string &name, const glm::vec3 &value);
    void setUniformMatrix4fv(const std::string &name, const glm::mat4 &matrix);
    void setUniform3fv(const std::string &name, const float *values);
    void setMat4(const std::string &name, const glm::mat4 &matrix) const;
};

#endif
