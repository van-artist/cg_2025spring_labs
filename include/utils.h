#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>
#include "Camera.h"

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window, Camera &camera, float deltaTime);
GLFWwindow *initWindow(unsigned int width, unsigned int height);
GLFWwindow *initWindow(unsigned int width, unsigned int height, std::string windowTitle);
void terminateGLFW(GLFWwindow *window);