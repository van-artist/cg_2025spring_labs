#version 330 core
layout (location = 0) in vec4 vertex; // vertex.xy 是屏幕位置（像素），vertex.zw 是纹理坐标
out vec2 TexCoords;

uniform mat4 projection;  // 正交投影矩阵，用于将屏幕坐标转换到标准化设备坐标

void main()
{
    gl_Position = projection * vec4(vertex.xy, 0.0, 1.0);
    TexCoords = vertex.zw;
}
