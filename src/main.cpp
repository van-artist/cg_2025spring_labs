#include <glad/glad.h>
#include <GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <iostream>
#include "ShaderLoader.h"
#include "utils.h"

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

int main()
{

    auto window = initWindow(SCR_WIDTH, SCR_HEIGHT, "BigCousin CG Lab");

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    ShaderLoader shaderLoader;
    auto shaderProgram = shaderLoader.createShaderProgram("./shaders/vertex_shader.vs", "./shaders/fragment_shader.fs");
    float vertices[] = {
        // 位置坐标       | 颜色             | 纹理坐标
        0.0f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.5f, 0.0f,    // 顶点 A
        -0.5f, -0.35f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // 顶点 B
        0.5f, -0.35f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,  // 顶点 C

        0.0f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.5f, 0.0f,   // 顶点 A
        0.5f, -0.35f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, // 顶点 C
        0.0f, 0.0f, 0.5f, 0.0f, 1.0f, 0.0f, 0.5f, 1.0f,   // 顶点 D

        0.0f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.5f, 0.0f,    // 顶点 A
        0.0f, 0.0f, 0.5f, 0.0f, 0.0f, 1.0f, 0.5f, 1.0f,    // 顶点 D
        -0.5f, -0.35f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, // 顶点 B

        -0.5f, -0.35f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, // 顶点 B
        0.5f, -0.35f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f,  // 顶点 C
        0.0f, 0.0f, 0.5f, 1.0f, 1.0f, 0.0f, 0.5f, 0.0f     // 顶点 D
    };

    unsigned int indices[] = {
        0, 1, 2,
        0, 2, 3,
        0, 1, 3,
        1, 2, 3};

    // 纹理操作
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    // 为当前绑定的纹理对象设置环绕、过滤方式
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // 加载并生成纹理
    int width, height, nrChannels;
    unsigned char *data = stbi_load("./static/images/image.jpeg", &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        float timeValue = glfwGetTime();
        float greenValue = (sin(timeValue) / 2.0f) + 0.5f;
        int vertexColorLocation = glGetUniformLocation(shaderProgram, "ourColor");

        glUseProgram(shaderProgram);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        glUniform1i(glGetUniformLocation(shaderProgram, "ourTexture"), 0);

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 12);
        glBindVertexArray(0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    glDeleteProgram(shaderProgram);

    glfwTerminate();
    return 0;
}
