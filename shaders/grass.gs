#version 330 core
layout(points) in;
layout(triangle_strip, max_vertices = 8) out;

in VS_OUT {
    vec3 color;
    vec4 worldPos;
} gs_in[];

out vec3 fColor;

uniform mat4 view;
uniform mat4 projection;

void main()
{
    vec4 center = gs_in[0].worldPos;
    fColor = gs_in[0].color;

    float size = 0.3;
    vec3 right = normalize(vec3(view[0][0], view[1][0], view[2][0])) * size;
    vec3 up = vec3(0.0, size, 0.0);

    vec3 positions[4] = vec3[](
        center.xyz - right,
        center.xyz + right,
        center.xyz - right + up,
        center.xyz + right + up
    );

    // 面片 1：朝向相机
    gl_Position = projection * view * vec4(positions[0], 1.0); EmitVertex();
    gl_Position = projection * view * vec4(positions[1], 1.0); EmitVertex();
    gl_Position = projection * view * vec4(positions[2], 1.0); EmitVertex();
    gl_Position = projection * view * vec4(positions[3], 1.0); EmitVertex();
    EndPrimitive();

    // 面片 2：旋转 90 度方向的横向草叶
    vec3 right2 = normalize(cross(right, up)) * size;

    vec3 p1 = center.xyz - right2;
    vec3 p2 = center.xyz + right2;
    vec3 p3 = p1 + up;
    vec3 p4 = p2 + up;

    gl_Position = projection * view * vec4(p1, 1.0); EmitVertex();
    gl_Position = projection * view * vec4(p2, 1.0); EmitVertex();
    gl_Position = projection * view * vec4(p3, 1.0); EmitVertex();
    gl_Position = projection * view * vec4(p4, 1.0); EmitVertex();
    EndPrimitive();
}
