#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "utils.h"
#include "Camera.h"
#include "Shader.h"
#include "Model.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <vector>
#include <iostream>

constexpr unsigned int SCR_WIDTH = 800;
constexpr unsigned int SCR_HEIGHT = 600;

/* -------------------------------------------------------------------------- */
/*  摄像机与时间                                                              */
/* -------------------------------------------------------------------------- */
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH * 0.5f, lastY = SCR_HEIGHT * 0.5f;
bool firstMouse = true;
float deltaTime = 0.0f, lastFrame = 0.0f;

/* -------------------------------------------------------------------------- */
/*  光源                                                                      */
/* -------------------------------------------------------------------------- */
glm::vec3 pointLightPositions[4] = {
    {5.0f, 1.0f, 5.0f},
    {-5.0f, 1.0f, 5.0f},
    {5.0f, 1.0f, -5.0f},
    {-5.0f, 1.0f, -5.0f}};

/* -------------------------------------------------------------------------- */
/*  顶点数据                                                                  */
/* -------------------------------------------------------------------------- */
float floorVertices[] = {
    10.0f, -0.5f, 10.0f, 5.0f, 0.0f,
    -10.0f, -0.5f, 10.0f, 0.0f, 0.0f,
    -10.0f, -0.5f, -10.0f, 0.0f, 5.0f,

    10.0f, -0.5f, 10.0f, 5.0f, 0.0f,
    -10.0f, -0.5f, -10.0f, 0.0f, 5.0f,
    10.0f, -0.5f, -10.0f, 5.0f, 5.0f};

float skyboxVertices[] = {/* 36 * 3 */
                          -1.0f, 1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f,
                          1.0f, -1.0f, -1.0f, 1.0f, 1.0f, -1.0f, -1.0f, 1.0f, -1.0f,

                          -1.0f, -1.0f, 1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f, -1.0f,
                          -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 1.0f, -1.0f, -1.0f, 1.0f,

                          1.0f, -1.0f, -1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
                          1.0f, 1.0f, 1.0f, 1.0f, 1.0f, -1.0f, 1.0f, -1.0f, -1.0f,

                          -1.0f, -1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
                          1.0f, 1.0f, 1.0f, 1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f,

                          -1.0f, 1.0f, -1.0f, 1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 1.0f,
                          1.0f, 1.0f, 1.0f, -1.0f, 1.0f, 1.0f, -1.0f, 1.0f, -1.0f,

                          -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f, 1.0f, -1.0f, -1.0f,
                          1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f, 1.0f, -1.0f, 1.0f};

std::vector<float> grassPoints = {
    -1.8f, -0.5f, 1.2f, 0.3f, 0.85f, 0.3f,
    1.2f, -0.5f, -1.4f, 0.2f, 0.75f, 0.2f,
    0.0f, -0.5f, 0.0f, 0.4f, 0.9f, 0.4f,
    -2.0f, -0.5f, -1.6f, 0.25f, 0.8f, 0.25f,
    2.3f, -0.5f, 0.9f, 0.3f, 0.9f, 0.3f,
    0.8f, -0.5f, 2.1f, 0.35f, 0.85f, 0.35f};

/* -------------------------------------------------------------------------- */
/*  工具函数                                                                  */
/* -------------------------------------------------------------------------- */
unsigned int loadTexture(const char *path, bool alpha = false)
{
    stbi_set_flip_vertically_on_load(false);
    int w, h, n;
    unsigned char *data = stbi_load(path, &w, &h, &n, 0);
    if (!data)
    {
        std::cerr << "Failed load " << path << "\n";
        return 0;
    }

    GLenum fmt = alpha || n == 4 ? GL_RGBA : (n == 3 ? GL_RGB : GL_RED);

    unsigned int tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexImage2D(GL_TEXTURE_2D, 0, fmt, w, h, 0, fmt, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    stbi_image_free(data);
    return tex;
}

unsigned int loadCubemap(const std::vector<std::string> &faces)
{
    unsigned int id;
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_CUBE_MAP, id);
    int w, h, n;
    for (size_t i = 0; i < faces.size(); ++i)
    {
        unsigned char *data = stbi_load(faces[i].c_str(), &w, &h, &n, 0);
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        stbi_image_free(data);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    return id;
}

/* -------------------------------------------------------------------------- */
void processInput(GLFWwindow *win, float dt)
{
    if (glfwGetKey(win, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(win, true);
    if (glfwGetKey(win, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboardInput(FORWARD, dt);
    if (glfwGetKey(win, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboardInput(BACKWARD, dt);
    if (glfwGetKey(win, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboardInput(LEFT, dt);
    if (glfwGetKey(win, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboardInput(RIGHT, dt);
}

void mouse_callback(GLFWwindow *, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }
    float dx = xpos - lastX, dy = lastY - ypos;
    lastX = xpos;
    lastY = ypos;
    camera.ProcessMouseMovement(dx, dy);
}
void scroll_callback(GLFWwindow *, double, double y) { camera.ProcessMouseScroll((float)y); }

/* ========================================================================== */
int main()
{
    GLFWwindow *window = initWindow(SCR_WIDTH, SCR_HEIGHT, "Scene");
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    /* =======================  顶点对象  =================================== */
    unsigned int floorVAO, floorVBO, skyVAO, skyVBO, grassVAO, grassVBO;

    /* --- floor --- */
    glGenVertexArrays(1, &floorVAO);
    glGenBuffers(1, &floorVBO);
    glBindVertexArray(floorVAO);
    glBindBuffer(GL_ARRAY_BUFFER, floorVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(floorVertices), floorVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    /* --- skybox --- */
    glGenVertexArrays(1, &skyVAO);
    glGenBuffers(1, &skyVBO);
    glBindVertexArray(skyVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    /* --- grass points --- */
    glGenVertexArrays(1, &grassVAO);
    glGenBuffers(1, &grassVBO);
    glBindVertexArray(grassVAO);
    glBindBuffer(GL_ARRAY_BUFFER, grassVBO);
    glBufferData(GL_ARRAY_BUFFER, grassPoints.size() * sizeof(float), grassPoints.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    /* =======================  材质与贴图  ================================ */
    unsigned int floorTex = loadTexture("static/textures/metal.png");
    unsigned int grassTex = loadTexture("static/textures/grass.png", true); // alpha!
    unsigned int skyTex = loadCubemap({"static/skybox/right.jpg", "static/skybox/left.jpg",
                                       "static/skybox/top.jpg", "static/skybox/bottom.jpg",
                                       "static/skybox/front.jpg", "static/skybox/back.jpg"});

    /* =======================  着色器  ==================================== */
    Shader floorShader("shaders/tex_only.vs", "shaders/tex_only.fs");
    Shader skyShader("shaders/skybox.vs", "shaders/skybox.fs");
    Shader grassShader("shaders/grass.vs", "shaders/grass.fs", "shaders/grass.gs");
    Shader modelShader("shaders/model_vertex_shader.vs", "shaders/model_fragment_shader.fs");

    floorShader.use();
    floorShader.setInt("texture1", 0);
    grassShader.use();
    grassShader.setInt("texture1", 0);
    skyShader.use();
    skyShader.setInt("skybox", 0);

    Model backpack("static/models/backpack/backpack.obj");

    /* =======================  循环  ====================================== */
    while (!glfwWindowShouldClose(window))
    {
        float now = (float)glfwGetTime();
        deltaTime = now - lastFrame;
        lastFrame = now;
        processInput(window, deltaTime);

        /* -----------  清屏  ----------- */
        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom),
                                                (float)SCR_WIDTH / SCR_HEIGHT, 0.1f, 300.0f);
        glm::mat4 view = camera.GetViewMatrix();

        /* -----------  地面  ----------- */
        floorShader.use();
        floorShader.setMat4("projection", projection);
        floorShader.setMat4("view", view);
        floorShader.setMat4("model", glm::mat4(1.0f));
        glBindVertexArray(floorVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, floorTex);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        /* -----------  草  -------------- */
        grassShader.use();
        grassShader.setMat4("projection", projection);
        grassShader.setMat4("view", view);
        grassShader.setMat4("model", glm::mat4(1.0f));
        glBindVertexArray(grassVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, grassTex);
        glDrawArrays(GL_POINTS, 0, (GLsizei)(grassPoints.size() / 6));

        /* -----------  背包模型 -------- */
        modelShader.use();
        modelShader.setInt("materialMode", 2);
        modelShader.setVec3("viewPos", camera.Position);
        modelShader.setMat4("projection", projection);
        modelShader.setMat4("view", view);
        glm::mat4 m = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.3f, 0.0f));
        m = glm::scale(m, glm::vec3(0.5f));
        m = glm::rotate(m, glm::radians(180.f), glm::vec3(0, 1, 0));
        modelShader.setMat4("model", m);
        for (int i = 0; i < 4; ++i)
        {
            std::string idx = std::to_string(i);
            modelShader.setVec3("pointLights[" + idx + "].position", pointLightPositions[i]);
            modelShader.setVec3("pointLights[" + idx + "].ambient", glm::vec3(0.2f));
            modelShader.setVec3("pointLights[" + idx + "].diffuse", glm::vec3(2.0f));
            modelShader.setVec3("pointLights[" + idx + "].specular", glm::vec3(3.0f));
            modelShader.setFloat("pointLights[" + idx + "].constant", 1.0f);
            modelShader.setFloat("pointLights[" + idx + "].linear", 0.022f);
            modelShader.setFloat("pointLights[" + idx + "].quadratic", 0.0019f);
        }
        backpack.Draw(modelShader);

        /* -----------  天空盒  ---------- */
        glDepthFunc(GL_LEQUAL);
        skyShader.use();
        skyShader.setMat4("projection", projection);
        skyShader.setMat4("view", glm::mat4(glm::mat3(view))); // 去掉平移
        glBindVertexArray(skyVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, skyTex);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glDepthFunc(GL_LESS);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &floorVAO);
    glDeleteBuffers(1, &floorVBO);
    glDeleteVertexArrays(1, &skyVAO);
    glDeleteBuffers(1, &skyVBO);
    glDeleteVertexArrays(1, &grassVAO);
    glDeleteBuffers(1, &grassVBO);
    terminateGLFW(window);
    return 0;
}
