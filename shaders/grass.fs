#version 330 core
in GS_OUT {
    vec3 color;
    vec2 TexCoord;
} fs_in;

uniform sampler2D texture1;  

out vec4 FragColor;

void main()
{
    vec4 tex = texture(texture1, fs_in.TexCoord);
    if (tex.a < 0.1) discard;              
    FragColor = tex * vec4(fs_in.color, 1.0); 
}
