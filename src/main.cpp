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

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f, lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;
float deltaTime = 0.0f, lastFrame = 0.0f;
int currentMaterialMode = 2;

glm::vec3 pointLightPositions[4] = {
    {5.0f, 1.0f, 5.0f},
    {-5.0f, 1.0f, 5.0f},
    {5.0f, 1.0f, -5.0f},
    {-5.0f, 1.0f, -5.0f}};

float cubeVertices[] = {
    // positions          // texture Coords
    -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,
    0.5f, -0.5f, -0.5f, 1.0f, 0.0f,
    0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
    0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
    -0.5f, 0.5f, -0.5f, 0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,

    -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
    0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
    0.5f, 0.5f, 0.5f, 1.0f, 1.0f,
    0.5f, 0.5f, 0.5f, 1.0f, 1.0f,
    -0.5f, 0.5f, 0.5f, 0.0f, 1.0f,
    -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,

    -0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
    -0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
    -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
    -0.5f, 0.5f, 0.5f, 1.0f, 0.0f,

    0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
    0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
    0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
    0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
    0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
    0.5f, 0.5f, 0.5f, 1.0f, 0.0f,

    -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
    0.5f, -0.5f, -0.5f, 1.0f, 1.0f,
    0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
    0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
    -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,

    -0.5f, 0.5f, -0.5f, 0.0f, 1.0f,
    0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
    0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
    0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
    -0.5f, 0.5f, 0.5f, 0.0f, 0.0f,
    -0.5f, 0.5f, -0.5f, 0.0f, 1.0f};
float floorVertices[] = {

    10.0f, -0.5f, 10.0f, 5.0f, 0.0f,
    -10.0f, -0.5f, 10.0f, 0.0f, 0.0f,
    -10.0f, -0.5f, -10.0f, 0.0f, 5.0f,

    10.0f, -0.5f, 10.0f, 5.0f, 0.0f,
    -10.0f, -0.5f, -10.0f, 0.0f, 5.0f,
    10.0f, -0.5f, -10.0f, 5.0f, 5.0f};

float skyboxVertices[] = {
    // positions
    -1.0f, 1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,
    1.0f, 1.0f, -1.0f,
    -1.0f, 1.0f, -1.0f,

    -1.0f, -1.0f, 1.0f,
    -1.0f, -1.0f, -1.0f,
    -1.0f, 1.0f, -1.0f,
    -1.0f, 1.0f, -1.0f,
    -1.0f, 1.0f, 1.0f,
    -1.0f, -1.0f, 1.0f,

    1.0f, -1.0f, -1.0f,
    1.0f, -1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f, 1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,

    -1.0f, -1.0f, 1.0f,
    -1.0f, 1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f, -1.0f, 1.0f,
    -1.0f, -1.0f, 1.0f,

    -1.0f, 1.0f, -1.0f,
    1.0f, 1.0f, -1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    -1.0f, 1.0f, 1.0f,
    -1.0f, 1.0f, -1.0f,

    -1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f, 1.0f,
    1.0f, -1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f, 1.0f,
    1.0f, -1.0f, 1.0f};

std::vector<float> grassPoints = {
    -1.0f, -0.5f, -1.0f, 0.3f, 0.9f, 0.3f,
    1.0f, -0.5f, 0.5f, 0.2f, 0.8f, 0.2f,
    0.0f, -0.5f, 1.5f, 0.4f, 0.9f, 0.4f,
    -1.5f, -0.5f, 2.0f, 0.3f, 0.7f, 0.3f,
    1.5f, -0.5f, -2.5f, 0.2f, 0.6f, 0.2f,
    0.5f, -0.5f, -1.8f, 0.3f, 0.9f, 0.3f};

unsigned int cubeVAO, VBO, lightCubeVAO, floorVAO, floorVBO, skyboxVAO, skyboxVBO, floorTexture, skyboxTexture;
unsigned int grassVAO, grassVBO;

unsigned int loadTexture(const char *path)
{
    unsigned int tex;
    glGenTextures(1, &tex);
    int w, h, n;
    unsigned char *data = stbi_load(path, &w, &h, &n, 0);
    GLenum fmt = n == 1 ? GL_RED : n == 3 ? GL_RGB
                                          : GL_RGBA;
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexImage2D(GL_TEXTURE_2D, 0, fmt, w, h, 0, fmt, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    stbi_image_free(data);
    return tex;
}

unsigned int loadCubemap(std::vector<std::string> faces)
{
    unsigned int id;
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_CUBE_MAP, id);
    int w, h, n;
    for (unsigned int i = 0; i < faces.size(); ++i)
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
    if (glfwGetKey(win, GLFW_KEY_1) == GLFW_PRESS)
        currentMaterialMode = 0;
    if (glfwGetKey(win, GLFW_KEY_2) == GLFW_PRESS)
        currentMaterialMode = 1;
    if (glfwGetKey(win, GLFW_KEY_3) == GLFW_PRESS)
        currentMaterialMode = 2;
}

void mouse_callback(GLFWwindow *, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }
    float xoff = xpos - lastX, yoff = lastY - ypos;
    lastX = xpos;
    lastY = ypos;
    camera.ProcessMouseMovement(xoff, yoff);
}
void scroll_callback(GLFWwindow *, double, double yoff) { camera.ProcessMouseScroll((float)yoff); }

int main()
{
    GLFWwindow *window = initWindow(SCR_WIDTH, SCR_HEIGHT, "Scene");
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    glEnable(GL_DEPTH_TEST);

    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(cubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glGenVertexArrays(1, &floorVAO);
    glGenBuffers(1, &floorVBO);
    glBindVertexArray(floorVAO);
    glBindBuffer(GL_ARRAY_BUFFER, floorVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(floorVertices), floorVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    glGenVertexArrays(1, &grassVAO);
    glGenBuffers(1, &grassVBO);
    glBindVertexArray(grassVAO);
    glBindBuffer(GL_ARRAY_BUFFER, grassVBO);
    glBufferData(GL_ARRAY_BUFFER, grassPoints.size() * sizeof(float), &grassPoints[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    floorTexture = loadTexture("static/textures/metal.png");
    std::vector<std::string> faces = {
        "static/skybox/right.jpg",
        "static/skybox/left.jpg",
        "static/skybox/top.jpg",
        "static/skybox/bottom.jpg",
        "static/skybox/front.jpg",
        "static/skybox/back.jpg"};
    stbi_set_flip_vertically_on_load(false);
    skyboxTexture = loadCubemap(faces);

    Shader modelShader("shaders/model_vertex_shader.vs", "shaders/model_fragment_shader.fs");
    Shader floorShader("shaders/tex_only.vs", "shaders/tex_only.fs");
    Shader skyShader("shaders/skybox.vs", "shaders/skybox.fs");
    Shader grassShader("shaders/grass.vs", "shaders/grass.fs", "shaders/grass.gs");

    floorShader.use();
    floorShader.setInt("texture1", 0);
    skyShader.use();
    skyShader.setInt("skybox", 0);

    Model backpack("static/models/backpack/backpack.obj");

    while (!glfwWindowShouldClose(window))
    {
        float current = (float)glfwGetTime();
        deltaTime = current - lastFrame;
        lastFrame = current;
        processInput(window, deltaTime);

        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / SCR_HEIGHT, 0.1f, 300.0f);
        glm::mat4 view = camera.GetViewMatrix();

        // floor
        floorShader.use();
        floorShader.setMat4("projection", projection);
        floorShader.setMat4("view", view);
        floorShader.setMat4("model", glm::mat4(1.0f));
        glBindVertexArray(floorVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, floorTexture);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // grass
        grassShader.use();
        grassShader.setMat4("projection", projection);
        grassShader.setMat4("view", view);
        grassShader.setMat4("model", glm::mat4(1.0f));
        glBindVertexArray(grassVAO);
        glDrawArrays(GL_POINTS, 0, grassPoints.size() / 6);

        // backpack
        modelShader.use();
        modelShader.setInt("materialMode", currentMaterialMode);
        modelShader.setVec3("viewPos", camera.Position);
        modelShader.setMat4("projection", projection);
        modelShader.setMat4("view", view);
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.3f, 0.0f));
        model = glm::scale(model, glm::vec3(0.5f));
        model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        modelShader.setMat4("model", model);

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

        // skybox
        glDepthFunc(GL_LEQUAL);
        glDepthMask(GL_FALSE);
        skyShader.use();
        skyShader.setMat4("projection", projection);
        skyShader.setMat4("view", glm::mat4(glm::mat3(view)));
        glBindVertexArray(skyboxVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glDepthMask(GL_TRUE);
        glDepthFunc(GL_LESS);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteVertexArrays(1, &floorVAO);
    glDeleteVertexArrays(1, &skyboxVAO);
    glDeleteVertexArrays(1, &grassVAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &floorVBO);
    glDeleteBuffers(1, &skyboxVBO);
    glDeleteBuffers(1, &grassVBO);

    terminateGLFW(window);
    return 0;
}
