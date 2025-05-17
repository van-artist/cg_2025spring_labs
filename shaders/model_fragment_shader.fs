#version 330 core

out vec4 FragColor;   // 输出颜色

in vec3 FragPos;      // 片段位置
in vec3 Normal;       // 片段法线
in vec2 TexCoords;    // 片段纹理坐标

uniform vec3 lightPos;    // 光源位置
uniform vec3 viewPos;     // 观察者位置
uniform sampler2D texture1; // 纹理采样器

void main()
{
    // 环境光
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * vec3(1.0, 1.0, 1.0);

    // 漫反射
    vec3 norm = normalize(Normal);  // 法线向量
    vec3 lightDir = normalize(lightPos - FragPos);  // 光源方向
    float diff = max(dot(norm, lightDir), 0.0);  // 漫反射分量
    vec3 diffuse = diff * vec3(1.0, 1.0, 1.0);  // 漫反射光

    // 镜面反射
    float specularStrength = 0.5;
    vec3 viewDir = normalize(viewPos - FragPos);  // 视线方向
    vec3 reflectDir = reflect(-lightDir, norm);   // 反射光线方向
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * vec3(1.0, 1.0, 1.0);  // 镜面反射光

    // 计算最终颜色
    vec3 result = (ambient + diffuse + specular) * texture(texture1, TexCoords).rgb;

    FragColor = vec4(result, 1.0);  // 输出最终的颜色
}
