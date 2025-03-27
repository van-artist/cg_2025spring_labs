# CG Lab1

---

姓名：刘尚
学号：202211010058

---

## 环境配置

### 系统环境

- MacOS
- 处理器架构：ARM64（Apple Silicon）
- 编译器: Clang（Apple Clang 15.0.0）

### 依赖库安装

在 macOS 上使用 Homebrew 安装 OpenGL 相关依赖库：

```bash
brew install cmake glew
```

#### glad 配置

下载 glad 源码，将 glad.h 放置在项目的 include/glad 目录下，将 glad.c 放置在项目的 src 目录下。

### 项目环境搭建（基于 Cmake）

CmakeLists.txt

```cmake
cmake_minimum_required(VERSION 3.10)
project(OpenGL_Project)

set(CMAKE_CXX_STANDARD 17)

# 查找 OpenGL
find_package(OpenGL REQUIRED)

# 添加 glad、GLEW、GLFW 头文件路径
include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/include/glad)
include_directories(/opt/homebrew/include)
link_directories(/opt/homebrew/lib)

file(GLOB SOURCES ${PROJECT_SOURCE_DIR}/src/*.cpp ${PROJECT_SOURCE_DIR}/src/glad.c)


# 生成可执行文件
add_executable(OpenGL_Project ${SOURCES})
add_custom_command(TARGET OpenGL_Project POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${PROJECT_SOURCE_DIR}/shaders $<TARGET_FILE_DIR:OpenGL_Project>/shaders
)
add_custom_command(TARGET OpenGL_Project POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${PROJECT_SOURCE_DIR}/static $<TARGET_FILE_DIR:OpenGL_Project>/static
)

# 链接 OpenGL、GLEW、GLFW
target_link_libraries(OpenGL_Project OpenGL::GL GLEW glfw)
```

#### 说明

shader/目录下通过文件单独编写着色器，在 Cmake 中添加了拷贝文件的命令，将着色器文件拷贝到可执行文件目录下。
同理，static/目录下存放纹理文件，也会被拷贝到可执行文件目录下。
Cmake 中添加了 GLFW 的链接，并且将 include 目录添加到头文件搜索路径中，可以实现在源文件中直接引用 GLFW 和 GLAD 的头文件。

### 编译 & 运行

在终端中执行：

```bash
mkdir build && cd build
cmake ..
make
./OpenGL_Project
```

## 任务实现

### 着色器加载器

之前提到，着色器文件通过 Cmake 拷贝到可执行文件目录下，现在实现了一个着色器加载器，用于加载着色器文件。

类定义如下

```cpp
class ShaderLoader
{
private:
    static std::string readShaderFile(const std::string &filePath);
    static void checkShaderCompilation(unsigned int shader, const std::string &type);
    static void checkProgramLinking(unsigned int program);

public:
    ShaderLoader();
    ~ShaderLoader();

    static unsigned int loadVertexShader(const std::string &filePath);
    static unsigned int loadFragmentShader(const std::string &filePath);
    static unsigned int compileShader(unsigned int type, const std::string &source);

    static unsigned int createShaderProgram(const std::string &vertexPath, const std::string &fragmentPath);

    static void deleteShader(unsigned int shader);
    static void deleteProgram(unsigned int program);
};
```

主要思路是通过 `glCreateShader` 创建着色器对象，通过 `glShaderSource` 和 `glCompileShader` 编译着色器，通过 `glCreateProgram` 创建着色器程序，通过 `glAttachShader` 和 `glLinkProgram` 链接着色器程序。

通过封装这些操作，可以方便地加载着色器文件，并且通过文件系统管理着色器文件。

详情代码可见代码仓库内容。

### 任务 1

在 hello_triangle.cpp 的基础上，实现立方体的渲染（最小的立方体为正四面体，由四
个顶点、四个三角形组成）

### 任务 2

在任务 1 的基础上，为不同三角形面设计不同的颜色，至少一个三角形的三个顶点颜
色不同

### 任务 3

在任务 2 的基础上，自选纹理图像，为立方体应用纹理
