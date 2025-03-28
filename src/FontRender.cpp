#include <ft2build.h>
#include <iostream>
#include FT_FREETYPE_H
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <filesystem>
#include "FontRender.h"

FontRender::FontRender(const std::string ttsPath, FT_UInt font_size)
{
    if (!std::filesystem::exists(ttsPath))
    {
        std::cerr << "Font path does not exist: " << ttsPath << std::endl;
        std::exit(EXIT_FAILURE);
    }
    std::cout << "Loading fonts: " << ttsPath << std::endl;
    FT_Library ft;
    if (FT_Init_FreeType(&ft))
    {
        std::cerr << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
        std::exit(EXIT_FAILURE);
    }

    FT_Face face;
    if (FT_New_Face(ft, ttsPath.c_str(), 0, &face))
    {
        std::cerr << "ERROR::FREETYPE: Failed to load font" << std::endl;
        std::exit(EXIT_FAILURE);
    }

    FT_Set_Pixel_Sizes(face, 0, font_size);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // 禁用字节对齐限制
    for (GLubyte c = 0; c < 128; c++)
    {
        // 加载字符的字形
        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
        {
            std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
            continue;
        }
        // 生成纹理
        GLuint texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RED,
            face->glyph->bitmap.width,
            face->glyph->bitmap.rows,
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            face->glyph->bitmap.buffer);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        Character character = {
            texture,
            glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
            glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
            face->glyph->advance.x};
        characters.insert(std::pair<GLchar, Character>(c, character));
    }
    // 清理 FreeType 资源
    FT_Done_Face(face);
    FT_Done_FreeType(ft);

    glGenVertexArrays(1, &textVAO);
    glGenBuffers(1, &textVBO);
    glBindVertexArray(textVAO);
    glBindBuffer(GL_ARRAY_BUFFER, textVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, nullptr, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

FontRender::~FontRender()
{
    for (auto &pair : characters)
    {
        glDeleteTextures(1, &pair.second.TextureID);
    }
}

GLuint FontRender::getCharacterTexture(GLchar c)
{
    if (characters.find(c) != characters.end())
    {
        return characters[c].TextureID;
    }
    else
    {
        std::cout << "Character not found!" << std::endl;
        return 0;
    }
}

void FontRender::renderText(ShaderLoader &shader, std::string text, float x, float y, float scale, glm::vec3 color)
{
    shader.use();
    glUniform3f(glGetUniformLocation(shader.getProgramID(), "textColor"), color.x, color.y, color.z);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(textVAO);

    // 保存初始 x 坐标，用于换行时重置
    float origX = x;

    float lineHeight = 48 * scale;

    for (char c : text)
    {
        if (c == '\n')
        {

            x = origX;
            y -= lineHeight;
            continue;
        }

        Character ch = characters[c];

        float xpos = x + ch.Bearing.x * scale;
        float ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

        float w = ch.Size.x * scale;
        float h = ch.Size.y * scale;

        float vertices[6][4] = {
            {xpos, ypos + h, 0.0f, 0.0f},
            {xpos, ypos, 0.0f, 1.0f},
            {xpos + w, ypos, 1.0f, 1.0f},

            {xpos, ypos + h, 0.0f, 0.0f},
            {xpos + w, ypos, 1.0f, 1.0f},
            {xpos + w, ypos + h, 1.0f, 0.0f}};

        glBindTexture(GL_TEXTURE_2D, ch.TextureID);
        glBindBuffer(GL_ARRAY_BUFFER, textVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        x += (ch.Advance >> 6) * scale;
    }

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}
