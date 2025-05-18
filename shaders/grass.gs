#version 330 core
layout (points)                          in;
layout (triangle_strip, max_vertices=4) out;

in VS_OUT {
    vec3 color;
    vec4 worldPos;
} gs_in[];

out GS_OUT {
    vec3 color;
    vec2 TexCoord;
} gs_out;

uniform mat4 view;
uniform mat4 projection;

void main()
{
    vec3 center = vec3(gs_in[0].worldPos);
    float size  = 0.35;

    vec3 right = normalize(vec3(view[0][0], view[1][0], view[2][0])) * size;
    vec3 up    = vec3(0.0, size, 0.0);

    gs_out.color = gs_in[0].color;
    gs_out.TexCoord = vec2(0.0, 1.0);
    gl_Position     = projection * view * vec4(center - right, 1.0);
    EmitVertex();

    gs_out.TexCoord = vec2(1.0, 1.0);
    gl_Position     = projection * view * vec4(center + right, 1.0);
    EmitVertex();

    gs_out.TexCoord = vec2(0.0, 0.0);
    gl_Position     = projection * view * vec4(center - right + up, 1.0);
    EmitVertex();

    gs_out.TexCoord = vec2(1.0, 0.0);
    gl_Position     = projection * view * vec4(center + right + up, 1.0);
    EmitVertex();

    EndPrimitive();
}
