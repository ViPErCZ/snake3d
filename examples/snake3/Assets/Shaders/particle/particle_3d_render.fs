#version 330 core

in vec4 vColor;
out vec4 FragColor;

uniform float u_colorSensitivity;

void main() {
    FragColor = vColor * u_colorSensitivity;
}
