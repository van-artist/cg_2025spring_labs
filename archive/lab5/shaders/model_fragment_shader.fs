#version 330 core

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform sampler2D texture1;
uniform vec3 viewPos;
uniform int materialMode;  // 0: only diffuse, 1: only specular, 2: both

#define NR_POINT_LIGHTS 4

struct PointLight {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

uniform PointLight pointLights[NR_POINT_LIGHTS];

void main()
{
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 texColor = texture(texture1, TexCoords).rgb;

    vec3 result = vec3(0.0);

    for (int i = 0; i < NR_POINT_LIGHTS; ++i)
    {
        // 光照方向与距离
        vec3 lightDir = normalize(pointLights[i].position - FragPos);
        float distance = length(pointLights[i].position - FragPos);
        float attenuation = 1.0 / (pointLights[i].constant +
                                   pointLights[i].linear * distance +
                                   pointLights[i].quadratic * distance * distance);

        // 环境光
        vec3 ambient = pointLights[i].ambient;

        // 漫反射
        float diff = max(dot(norm, lightDir), 0.0);
        vec3 diffuse = pointLights[i].diffuse * diff;

        // 镜面反射
        vec3 reflectDir = reflect(-lightDir, norm);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
        vec3 specular = pointLights[i].specular * spec;

        // 根据 materialMode 决定是否加入 diffuse / specular
        if (materialMode == 0)
            result += (ambient + diffuse) * texColor * attenuation;
        else if (materialMode == 1)
            result += (ambient + specular) * texColor * attenuation;
        else
            result += (ambient + diffuse + specular) * texColor * attenuation;
    }

    FragColor = vec4(result, 1.0);
}
