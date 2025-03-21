#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);
GLFWwindow *initWindow(unsigned int width, unsigned int height);
GLFWwindow *initWindow(unsigned int width, unsigned int height, std::string windowTitle);
