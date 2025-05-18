# Lab6

---

姓名：刘尚  
学号：202211010058  
代码仓库：[Github](https://github.com/van-artist/cg_2025spring_labs)

---

## 环境配置

我们仍然在原本实验基础上进行实验，无需新的程序运行时环境配置。

- 系统环境：MacOS
- 编译器：clang 15.0

### 项目结构调整

为了适配这次实验新的资源文件，我们需要对项目结构进行调整。同时为了增量修改，直接吧 learnopengl 给的`skybox`和`metal`的贴图资源文件夹`static`放在根目录
之前的实验已经做好了 cmake 拷贝资源文件的工作。

## 实验任务

- 任务 1：基于实验五的任务 3，结合“depth_testint.cpp”与”cubemaps_skybox.cpp”
  ，为 backpack
  模型增加简单的天空盒背景与带纹理的地板；
- 任务 2：基于任务 1，调整地面或模型的位置与大小，防止穿模并与场景适配，展示完整的正面
  静态场景；
- 任务 3：基于任务 2，结合”geometry_shader_houses.cpp”
  ，添加平面小草作为地面装饰，并
  展示完整的多视角静态场景（至少 3 个不同视角）。

## 任务实现

仍然是先进行整体的修改，然后针对每个任务进行说明，最终的实现效果在最后给出

### 任务 1

我们需要在原有的代码基础上进行修改，主要是增加天空盒和地板的纹理。我们可以使用 learnopengl 提供的天空盒和地板纹理资源。
先用示例代码给的全局资源

```cpp
unsigned int floorVAO = 0, floorVBO = 0;
unsigned int floorTexture;
unsigned int skyboxVAO = 0, skyboxVBO = 0;
unsigned int skyboxTexture;

float floorVertices[] = {
    // pos              // uv
     10.0f, -0.5f,  10.0f,  5.0f, 0.0f,
    -10.0f, -0.5f,  10.0f,  0.0f, 0.0f,
    -10.0f, -0.5f, -10.0f,  0.0f, 5.0f,

     10.0f, -0.5f,  10.0f,  5.0f, 0.0f,
    -10.0f, -0.5f, -10.0f,  0.0f, 5.0f,
     10.0f, -0.5f, -10.0f,  5.0f, 5.0f
};
```

还有`skyboxVertices`数组，直接复制到代码中，详情见`main.cpp`。

初始化所有资源

```cpp
glGenVertexArrays(1, &floorVAO);
glGenBuffers(1, &floorVBO);
glBindVertexArray(floorVAO);
glBindBuffer(GL_ARRAY_BUFFER, floorVBO);
glBufferData(GL_ARRAY_BUFFER, sizeof(floorVertices), floorVertices, GL_STATIC_DRAW);
glEnableVertexAttribArray(0);                // 位置
glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)0);
glEnableVertexAttribArray(1);                // UV
glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)(3*sizeof(float)));
glBindVertexArray(0);

glGenVertexArrays(1, &skyboxVAO);
glGenBuffers(1, &skyboxVBO);
glBindVertexArray(skyboxVAO);
glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);
glEnableVertexAttribArray(0);
glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);
glBindVertexArray(0);

floorTexture  = loadTexture("resources/textures/metal.png");

std::vector<std::string> faces{
    "resources/skybox/right.jpg",
    "resources/skybox/left.jpg",
    "resources/skybox/top.jpg",
    "resources/skybox/bottom.jpg",
    "resources/skybox/front.jpg",
    "resources/skybox/back.jpg"
};
skyboxTexture = loadCubemap(faces);

Shader texShader("shaders/tex_only.vs","shaders/tex_only.fs");   // 给地板用
Shader skyShader("shaders/skybox.vs"  ,"shaders/skybox.fs");     // 给天空盒用
texShader.use();  texShader.setInt("texture1", 0);
skyShader.use();  skyShader.setInt("skybox",   0);
```

搬运一下 learnopengl 建议的新增的着色器代码，`tex_only.vs` 和 `tex_only.fs`，以及 `skybox.vs` 和 `skybox.fs`。
注意这里的着色器代码和之前的有些不同，主要是增加了纹理采样的部分。详情见`shaders`目录。

渲染循环的修改，我们需要在渲染循环中添加地板和天空盒的渲染代码。我们需要在每一帧中渲染天空盒和地板。

```cpp
glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom),
                       SCR_WIDTH/(float)SCR_HEIGHT, 0.1f, 300.0f);
glm::mat4 view = camera.GetViewMatrix();

texShader.use();
texShader.setMat4("projection", projection);
texShader.setMat4("view", view);
glm::mat4 model = glm::mat4(1.0f);
texShader.setMat4("model", model);
glBindVertexArray(floorVAO);
glActiveTexture(GL_TEXTURE0);
glBindTexture(GL_TEXTURE_2D, floorTexture);
glDrawArrays(GL_TRIANGLES, 0, 6);
glBindVertexArray(0);

glDepthFunc(GL_LEQUAL);                           // 让深度=1 的片段通过
skyShader.use();
skyShader.setMat4("projection", projection);
glm::mat4 skyView = glm::mat4(glm::mat3(view));   // 去掉平移
skyShader.setMat4("view", skyView);
glBindVertexArray(skyboxVAO);
glActiveTexture(GL_TEXTURE0);
glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);
glDrawArrays(GL_TRIANGLES, 0, 36);
glBindVertexArray(0);
glDepthFunc(GL_LESS);                             // 还原

```

### 任务 2

我们需要调整地面和模型的位置和大小，防止穿模并与场景适配。可以通过调整模型的变换矩阵来实现。

```cpp
glm::mat4 model = glm::mat4(1.0f);
model = glm::translate(model, glm::vec3(0.0f, 0.3f, 0.0f));                    // 向上抬高
model = glm::scale(model, glm::vec3(0.5f));                                    // 缩小一半
model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // 旋转面向摄像机（如有需要）
modelShader.setMat4("model", model);
```

这样就可以正常显示了

### 任务 3

先搬运一波`geometry_shader_houses.cpp` 的配套着色器的代码，为了更好的区分，复制一份改成 grass 名称。

只需在主循环中额外添加草的渲染部分。
先集中设置一下草的位置和顶点数据。

```cpp
std::vector<float> grassPoints = {
    -1.0f, -0.5f, -1.0f, 0.3f, 0.9f, 0.3f,
     1.0f, -0.5f,  0.5f, 0.2f, 0.8f, 0.2f,
     0.0f, -0.5f,  1.5f, 0.4f, 0.9f, 0.4f,
    -1.5f, -0.5f,  2.0f, 0.3f, 0.7f, 0.3f,
     1.5f, -0.5f, -2.5f, 0.2f, 0.6f, 0.2f,
     0.5f, -0.5f, -1.8f, 0.3f, 0.9f, 0.3f
};
```

为了适应形状，修改着色器

grass.vs

```glsl
#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aColor;

out VS_OUT {
    vec3 color;
    vec4 worldPos;
} vs_out;

uniform mat4 model;

void main()
{
    vs_out.color = aColor;
    vs_out.worldPos = model * vec4(aPos, 1.0);
    gl_Position = vs_out.worldPos;
}
```

grass.gs

```glsl
#version 330 core
layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

in VS_OUT {
    vec3 color;
    vec4 worldPos;
} gs_in[];

out vec3 fColor;

uniform mat4 view;
uniform mat4 projection;

void main()
{
    vec4 p = gs_in[0].worldPos;
    float size = 0.3;
    fColor = gs_in[0].color;

    vec3 right = vec3(view[0][0], view[1][0], view[2][0]) * size;
    vec3 up    = vec3(0.0, size, 0.0);

    gl_Position = projection * view * vec4(p.xyz - right, 1.0); EmitVertex();
    gl_Position = projection * view * vec4(p.xyz + right, 1.0); EmitVertex();
    gl_Position = projection * view * vec4(p.xyz - right + up, 1.0); EmitVertex();
    gl_Position = projection * view * vec4(p.xyz + right + up, 1.0); EmitVertex();
    EndPrimitive();
}

```

grass.fs

```glsl
#version 330 core
in vec3 fColor;
out vec4 FragColor;

void main()
{
    FragColor = vec4(fColor, 1.0);
}

```

加载新的着色器并设置 VAO/VBO

```cpp
Shader grassShader("shaders/grass.vs", "shaders/grass.fs", "shaders/grass.gs");
unsigned int grassVAO, grassVBO;
glGenVertexArrays(1, &grassVAO);
glGenBuffers(1, &grassVBO);

glBindVertexArray(grassVAO);
glBindBuffer(GL_ARRAY_BUFFER, grassVBO);
glBufferData(GL_ARRAY_BUFFER, grassPositions.size() * sizeof(glm::vec3), &grassPositions[0], GL_STATIC_DRAW);
glEnableVertexAttribArray(0);
glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
```

然后在渲染循环中添加草的渲染代码

```cpp
grassShader.use();
grassShader.setMat4("projection", projection);
grassShader.setMat4("view", view);
glBindVertexArray(grassVAO);
glDrawArrays(GL_POINTS, 0, grassPositions.size());
```
