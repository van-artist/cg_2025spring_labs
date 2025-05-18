#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;

out VS_OUT {
    vec3  color;
    vec4  worldPos;
} vs_out;

uniform mat4 model;

void main()
{
    vs_out.color     = aColor;
    vs_out.worldPos  = model * vec4(aPos, 1.0);
    gl_Position      = vs_out.worldPos;        
}
