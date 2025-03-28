#version 330 core
out vec4 FragColor;
flat in vec3 faceColor;
in vec2 texCoord;

uniform sampler2D ourTexture;
uniform vec3 textColor;

void main() {
    vec4 texColor = texture(ourTexture, texCoord);
    FragColor = texColor * vec4(faceColor, 1.0);
}
