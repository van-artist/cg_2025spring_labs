# Lab5 模型加载与材质

---

姓名：刘尚  
学号：202211010058  
代码仓库：[Github](https://github.com/van-artist/cg_2025spring_labs)

---

## 环境配置

环境配置承接前三次实验，在原本的基础上，添加了对 Assimp 的支持
仍然使用 mac 的 homebrew 安装 Assimp

```shell
brew install assimp
```

查看路径

```shell
which assimp
```

![alt text](<截屏2025-05-18 19.24.48.png>)

增加 cmake 配置

```cmake
cmake_minimum_required(VERSION 3.10)
project(OpenGL_Project)

set(CMAKE_CXX_STANDARD 17)

# 设置 FreeType 库路径
set(FREETYPE_INCLUDE_DIRS "/opt/homebrew/opt/freetype/include/freetype2")
set(FREETYPE_LIBRARIES "/opt/homebrew/opt/freetype/lib/libfreetype.dylib")

# 查找 OpenGL
find_package(OpenGL REQUIRED)

# 查找 Assimp
find_package(ASSIMP REQUIRED)

# 添加头文件路径
include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(/opt/homebrew/include)
include_directories(${FREETYPE_INCLUDE_DIRS})
include_directories(${ASSIMP_INCLUDE_DIR})
link_directories(/opt/homebrew/lib)


file(GLOB SOURCES "${PROJECT_SOURCE_DIR}/src/*.cpp" "${PROJECT_SOURCE_DIR}/src/*.c")

add_definitions(-DGL_SILENCE_DEPRECATION)

# 生成可执行文件
add_executable(OpenGL_Project ${SOURCES})

# 添加着色器和其他文件的拷贝命令
add_custom_command(TARGET OpenGL_Project POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${PROJECT_SOURCE_DIR}/shaders $<TARGET_FILE_DIR:OpenGL_Project>/shaders
)
add_custom_command(TARGET OpenGL_Project POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${PROJECT_SOURCE_DIR}/static $<TARGET_FILE_DIR:OpenGL_Project>/static
)
add_custom_command(TARGET OpenGL_Project POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${PROJECT_SOURCE_DIR}/fonts $<TARGET_FILE_DIR:OpenGL_Project>/fonts
)

# 链接 OpenGL、GLEW、GLFW、Assimp 和 FreeType 库
target_link_libraries(OpenGL_Project OpenGL::GL GLEW glfw ${FREETYPE_LIBRARIES} ${ASSIMP_LIBRARIES})
```

现在可以使用 Assimp 了

## 实验任务

- 任务 1：在“model_loading.cpp”中修改相机视角或模型位置，使得在初始视图中可以观察到完整
  模型；
- 任务 2：基于任务 1，修改片段着色器代码，展示仅有漫反射材质、仅有镜面材质、二者均
  有的三种不同渲染效果；
- 任务 3：结合“light_casters_point.cpp”，基于任务 2 增加点光源，并修改光源配置，展示仅
  有漫反射材质、仅有镜面材质、二者均有的三种不同渲染效果。

## 任务实现

接下来先根据 learnopengl 的教程，封装实现各个类，然后在 main 中模仿`model_loading.cpp`和`light_casters_point.cpp`的结构，完成相应的功能。
如果实验的要求是递进的，总的实验效果会在最后展示。

### `Camera` 类完善

在原本的基础上，把事件响应的逻辑从外部封装到`Camera` 类中

```cpp
void ProcessKeyboard(Camera_Movement direction, float deltaTime);
void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true);
void ProcessMouseScroll(float yoffset);
void ProcessKeyboardInput(Camera_Movement direction, float deltaTime);
```

具体实现见代码`src/Camera.cpp`和`src/Camera.h`

### `Mesh` 类实现

`Mesh` 类是几何网格的抽象，封装了模型的基本组成单元（顶点、索引和纹理），并负责 VAO/VBO/EBO 的 OpenGL 缓存管理与绘制流程。

**主要原理：**

- **顶点封装**：  
   每个 `Vertex` 结构体包含位置、法线、纹理坐标、切线、副切线，以及最多 4 个骨骼索引和权重，支持基础的 PBR 材质系统和骨骼动画。

- **纹理绑定逻辑**：  
   遍历纹理数组，将其绑定到连续的 `GL_TEXTURE` 单元，并根据类型（diffuse/specular/normal/height）传入对应的着色器 uniform。

- **顶点属性指针**：  
   使用 `glVertexAttribPointer` 和 `glVertexAttribIPointer` 设置属性位置（位置、法线、纹理、切线、副切线、骨骼索引、权重）。

- **绘制调用**：  
   统一使用 `glDrawElements(GL_TRIANGLES)` 实现渲染。

---

### `Model` 类实现

`Model` 类用于封装整个模型的加载与管理流程，本质上由多个 `Mesh` 组成。实现充分利用 Assimp 库进行模型导入、节点遍历和材质处理。

**核心原理：**

- **模型加载流程**：  
   使用 `Assimp::Importer::ReadFile()` 读取模型文件（支持 obj/fbx 等格式），并开启三角化、法线生成、UV 翻转、切线计算等预处理选项。

- **递归解析节点树**：  
   `processNode()` 递归遍历 Assimp 的层次结构，对每个子节点执行 mesh 解析。

- **网格解析**：  
   `processMesh()` 提取顶点属性、面索引，并绑定材质信息。

- **纹理去重加载机制**：  
   使用 `textures_loaded` 记录已加载纹理路径，避免重复加载。利用 `stb_image` 完成图像解码，并上传至 OpenGL 纹理对象。

### 任务一实现

在前三次实验中，我们已经做好了 Camera 类的对视角控制的实现，启动程序后我们可以自由的调整相机的位置和方向，以便更好地观察加载的模型。即使这里使用了 Assimp 库来加载模型，我们也可以通过相机的视角来观察模型。

在`main.cpp`中，创建一个 `Camera` 对象，并在每一帧更新相机的视角。

总的实现效果会在最后展示

<video controls src="录屏2025-05-18 19.34.05.mov" title="Title"></video>

### 任务二实现

为了实现材质的不同渲染效果，我们其实只需修改片段着色器代码即可。
根据 learnopengl 的教程，修改片段着色器代码，展示仅有漫反射材质、仅有镜面材质、二者均有的三种不同渲染效果。
为了灵活和扩展性，我们在着色器中额外添加一个`materialMode` 的 uniform 变量，来控制渲染的材质类型。

片段着色器代码人如下：

```glsl
#version 330 core

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform sampler2D texture1;

// 新增：渲染模式控制变量（0=diffuse，1=specular，2=both）
uniform int materialMode;

void main()
{
    // 环境光
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * vec3(1.0);

    // 法线与方向
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);

    // 漫反射
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * vec3(1.0);

    // 镜面反射
    float specularStrength = 0.5;
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * vec3(1.0);

    // 纹理采样
    vec3 texColor = texture(texture1, TexCoords).rgb;

    // 根据模式选择渲染方式
    vec3 result;
    if (materialMode == 0)
        result = (ambient + diffuse) * texColor;            // 仅漫反射
    else if (materialMode == 1)
        result = (ambient + specular) * texColor;           // 仅镜面反射
    else
        result = (ambient + diffuse + specular) * texColor; // 两者都有

    FragColor = vec4(result, 1.0);
}
```

然后可以在`main.cpp`中设置不同的材质模式

```cpp
int currentMaterialMode = 2; // 默认漫反射+镜面反射
```

然后通过按键输入来控制显示效果:

```cpp
if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
    currentMaterialMode = 0;
if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
    currentMaterialMode = 1;
if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
    currentMaterialMode = 2;
```

先给`Shader`添加一个`setVec3` 方法，支持灵活设置 uniform 变量

```cpp
void setVec3(const std::string &name, float x, float y, float z) const;
void setVec3(const std::string &name, const glm::vec3 &value) const;
void Shader::setVec3(const std::string &name, float x, float y, float z) const
{
    glUniform3f(glGetUniformLocation(programID, name.c_str()), x, y, z);
}

void Shader::setVec3(const std::string &name, const glm::vec3 &value) const
{
    glUniform3f(glGetUniformLocation(programID, name.c_str()), value.x, value.y, value.z);
}

```

在每帧绘制前设置：

```cpp
shader.use();
shader.setInt("materialMode", currentMaterialMode);
shader.setVec3("viewPos", camera.Position);
shader.setVec3("lightPos", glm::vec3(10.0f, 10.0f, 10.0f));
```

来看静态效果:

- 漫反射 + 镜面反射
  ![alt text](<截屏2025-05-18 19.47.28.png>)
- 仅漫反射
  ![alt text](<截屏2025-05-18 19.47.23.png>)
- 仅镜面反射
  ![alt text](<截屏2025-05-18 19.47.18.png>)

可以发现：

- 漫反射 + 镜面反射的效果整体明亮且具有高光、阴影层次明显。
- 仅镜面反射的效果整体偏暗，仅有高光区域亮
- 仅漫反射的效果色彩正常但无明显高光反射）

### 任务三实现

为了支持多光源的渲染，我们需要增强着色器的能力，支持多个光源的渲染。
根据之前的经验，我们得升级我们的着色器，可以支持多个光源的渲染，同时为了动态的控制，我们要在程序中`Shader`类补充动态设置 uniform 变量的能力。

参考 learnopengl,我们可以在片段着色器中添加一个`lightCount` 的 uniform 变量，来控制渲染的光源数量，并且在循环中根据光源数量动态计算光照效果。
并且添加一个`PointLight` 结构体来描述光源的属性。

完善后的片段着色器代码如下：

```glsl
struct PointLight {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

#define NR_POINT_LIGHTS 4

uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform vec3 viewPos;
uniform sampler2D texture1;
uniform int materialMode;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

out vec4 FragColor;

void main()
{
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 result = vec3(0.0);

    for (int i = 0; i < NR_POINT_LIGHTS; ++i)
    {
        vec3 lightDir = normalize(pointLights[i].position - FragPos);
        float distance = length(pointLights[i].position - FragPos);
        float attenuation = 1.0 / (pointLights[i].constant + pointLights[i].linear * distance + pointLights[i].quadratic * (distance * distance));

        // 环境光
        vec3 ambient = pointLights[i].ambient;

        // 漫反射
        float diff = max(dot(norm, lightDir), 0.0);
        vec3 diffuse = pointLights[i].diffuse * diff;

        // 镜面反射
        vec3 reflectDir = reflect(-lightDir, norm);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
        vec3 specular = pointLights[i].specular * spec;

        if (materialMode == 0) result += ambient + diffuse;
        else if (materialMode == 1) result += ambient + specular;
        else result += ambient + diffuse + specular;

        result *= attenuation;
    }

    FragColor = vec4(result * texture(texture1, TexCoords).rgb, 1.0);
}
```

然后在`main.cpp`中的渲染中增加点光源，并尝试让光源移动

```cpp
//用四个vector3表示四个点光源的位置
glm::vec3 pointLightPositions[] = {
    glm::vec3(5.0f, 5.0f, 5.0f),
    glm::vec3(-5.0f, 5.0f, 5.0f),
    glm::vec3(5.0f, 5.0f, -5.0f),
    glm::vec3(-5.0f, 5.0f, -5.0f)
};
```

再给`Shader` 添加一个 `setInt` 和`setFloat`方法，支持灵活设置 uniform 变量

```cpp
void setInt(const std::string &name, int value) const;
void setFloat(const std::string &name, float value) const;
```

```cpp
void Shader::setInt(const std::string &name, int value)
{
    glUniform1i(glGetUniformLocation(programID, name.c_str()), value);
}
void Shader::setFloat(const std::string &name, float value) const
{
    glUniform1f(glGetUniformLocation(programID, name.c_str()), value);
}
```

然后传递光源参数到 Shader

```cpp
shader.use();
shader.setInt("materialMode", currentMaterialMode);
shader.setVec3("viewPos", camera.Position);

for (int i = 0; i < 4; ++i)
{
    std::string number = std::to_string(i);
    shader.setVec3("pointLights[" + number + "].position", pointLightPositions[i]);
    shader.setVec3("pointLights[" + number + "].ambient", glm::vec3(0.1f));
    shader.setVec3("pointLights[" + number + "].diffuse", glm::vec3(0.8f));
    shader.setVec3("pointLights[" + number + "].specular", glm::vec3(1.0f));
    shader.setFloat("pointLights[" + number + "].constant", 1.0f);
    shader.setFloat("pointLights[" + number + "].linear", 0.09f);
    shader.setFloat("pointLights[" + number + "].quadratic", 0.032f);
}
```

为了让发光源更明显，我们可以再添加一个光源立方体系统，独立写新的着色器

顶点着色器

```glsl
#version 330 core
layout(location=0) in vec3 aPos;
uniform mat4 model, view, projection;
void main() { gl_Position = projection*view*model*vec4(aPos,1.0); }
```

片段着色器

```glsl
#version 330 core
out vec4 FragColor;
uniform vec3 lightColor;
void main() { FragColor = vec4(lightColor,1.0); }
```

在`main.cpp`中添加光源立方体的 VAO/VBO/EBO 的 OpenGL 缓存管理与绘制流程

```cpp
unsigned int VBO, cubeVAO, lightCubeVAO;
createUnitCube(VBO, cubeVAO);
glGenVertexArrays(1,&lightCubeVAO);
glBindVertexArray(lightCubeVAO);
glBindBuffer(GL_ARRAY_BUFFER, VBO);
glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,8*sizeof(float),(void*)0);
glEnableVertexAttribArray(0);

```

加载着色器：

```cpp
Shader shader("shaders/model_vertex_shader.vs",
              "shaders/model_fragment_shader.fs");
Shader lightCubeShader("shaders/light_cube.vs",
                       "shaders/light_cube.fs");
```

顶点数据就用`light_casters_point.cpp`中的数据，详情见代码`src/main.cpp`

## 最终效果

实现了模型加载，视角移动，多光源，立方体光源可以在场景中自由移动，产生动态的光照效果后，最终的效果如下：
<video controls src="录屏2025-05-18 20.18.42.mov" title="Title"></video>
