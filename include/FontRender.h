#pragma once
#include <GLFW/glfw3.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <glm/glm.hpp>
#include <map>
#include <string>
#include "ShaderLoader.h"

struct Character
{
    GLuint TextureID;   // 字形纹理的ID
    glm::ivec2 Size;    // 字形大小
    glm::ivec2 Bearing; // 从基准线到字形左部/顶部的偏移值
    FT_Pos Advance;     // 原点距下一个字形原点的距离
};
using Characters = std::map<GLchar, Character>;

class FontRender
{
private:
    Characters characters;
    GLuint textVAO, textVBO;

public:
    FontRender(const std::string ttsPath, FT_UInt font_size = 48);
    GLuint getCharacterTexture(GLchar c);
    void renderText(ShaderLoader &shader, std::string text, float x, float y, float scale, glm::vec3 color);

    ~FontRender();
};