#version 330 core
in vec2 TexCoords;
out vec4 FragColor;

uniform sampler2D text;    // 文字纹理（通常是单通道，存储字形的 alpha 值）
uniform vec3 textColor;    // 文字颜色

void main()
{
    // 从纹理中采样，只取红色通道作为 alpha 值（因为字体纹理一般只存灰度信息）
    float alpha = texture(text, TexCoords).r;
    FragColor = vec4(textColor, alpha);
}
