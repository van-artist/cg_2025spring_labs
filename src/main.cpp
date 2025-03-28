#include <glad/glad.h>
#include <GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <sstream>
#include <ft2build.h>
#include FT_FREETYPE_H
#include "ShaderLoader.h"
#include "utils.h"
#include "Camera.h"
#include "FontRender.h"

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

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
float lastX = SCR_WIDTH / 2.0f; // 鼠标初始位置
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

Camera camera;

void mouse_callback(GLFWwindow *window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xOffset = xpos - lastX;
    float yOffset = lastY - ypos; // y轴反向
    lastX = xpos;
    lastY = ypos;

    camera.processMouseMovement(xOffset, yOffset); // 传递偏移量给摄像机
}

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
{
    camera.setFov(camera.getFov() - yoffset);

    if (camera.getFov() < 1.0f)
        camera.setFov(1.0f);
    if (camera.getFov() > 45.0f)
        camera.setFov(45.0f);
}

int main()
{
    auto window = initWindow(SCR_WIDTH, SCR_HEIGHT, "BigCousin CG Lab");
    FontRender fontRender("./fonts/Unicode.ttf");
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    glDisable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    ShaderLoader modelShader;
    modelShader.createShaderProgram("./shaders/model_vertex_shader.vs", "./shaders/model_fragment_shader.fs");
    auto shaderProgram = modelShader.getProgramID();
    ShaderLoader textShader;
    textShader.createShaderProgram("./shaders/text_vertex_shader.vs", "./shaders/text_fragment_shader.fs");
    auto textProgram = textShader.getProgramID();

    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height, nrChannels;
    uint16_t *data = (uint16_t *)stbi_load("./static/images/image.jpeg", &width, &height, &nrChannels, 0);
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

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::rotate(model, glm::radians(45.0f), glm::vec3(0.0, 1.0, 0.0));
    model = glm::scale(model, glm::vec3(1.1, 1.1, 1.1));

    unsigned int transformLoc = glGetUniformLocation(shaderProgram, "transform");
    if (transformLoc == -1)
    {
        std::cerr << "ERROR: Uniform 'transform' not found in shader!" << std::endl;
    }
    else
    {
        std::cout << "Uniform 'transform' location: " << transformLoc << std::endl;
    }

    while (!glfwWindowShouldClose(window))
    {
        processInput(window, camera);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 view = camera.getViewMatrix();
        glm::mat4 projection = camera.getProjectionMatrix((float)SCR_WIDTH / SCR_HEIGHT);

        float timeValue = glfwGetTime();
        float angle = timeValue * glm::radians(50.0f);

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::rotate(model, angle, glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(1.1, 1.1, 1.1));

        glm::mat4 trans = projection * view * model;

        modelShader.use();
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        glUniform1i(glGetUniformLocation(shaderProgram, "ourTexture"), 0);

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 12);
        glBindVertexArray(0);

        textShader.use();
        glm::mat4 ortho = glm::ortho(0.0f, (float)SCR_WIDTH, 0.0f, (float)SCR_HEIGHT);
        glUniformMatrix4fv(glGetUniformLocation(textShader.getProgramID(), "projection"), 1, GL_FALSE, glm::value_ptr(ortho));

        // 获取摄像机数据
        glm::vec3 camPos = camera.getPosition();
        glm::vec3 camFront = camera.getFront();
        glm::mat4 viewMatrix = camera.getViewMatrix();

        // 分段生成字符串
        std::string posText = "Camera Position: (" +
                              std::to_string(camPos.x) + ", " +
                              std::to_string(camPos.y) + ", " +
                              std::to_string(camPos.z) + ")";

        std::string dirText = "Camera Direction: (" +
                              std::to_string(camFront.x) + ", " +
                              std::to_string(camFront.y) + ", " +
                              std::to_string(camFront.z) + ")";

        // 生成视图矩阵的多行字符串
        std::stringstream ss;
        ss << "View Matrix:\n";
        for (int i = 0; i < 4; i++)
        {
            ss << viewMatrix[i][0] << " " << viewMatrix[i][1] << " "
               << viewMatrix[i][2] << " " << viewMatrix[i][3];
            if (i != 3)
                ss << "\n"; // 每行后换行，最后一行不需要
        }
        std::string viewText = ss.str();

        // 假设屏幕高度为 SCR_HEIGHT，这里设置起始 y 坐标（注意正交矩阵通常以左下角为原点）
        float startY = SCR_HEIGHT - 25.0f;

        // 渲染每段文字（文字渲染时使用正交投影矩阵，确保文字固定在屏幕上）
        fontRender.renderText(textShader, posText, 25.0f, startY, 0.5f, glm::vec3(1.0f, 1.0f, 1.0f));
        fontRender.renderText(textShader, dirText, 25.0f, startY - 25.0f, 0.5f, glm::vec3(1.0f, 1.0f, 1.0f));
        fontRender.renderText(textShader, viewText, 25.0f, startY - 50.0f, 0.5f, glm::vec3(1.0f, 1.0f, 1.0f));

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);

    glfwTerminate();
    return 0;
}
