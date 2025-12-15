#version 330 core

layout(location = 0) in vec3 aPos;

layout(location = 4) in mat4 instanceModel;
layout(location = 8) in vec4 instanceColor;

out vec4 vColor;

uniform mat4 view;
uniform mat4 projection;

void main() {
    vColor = instanceColor;
    gl_Position = projection * view * instanceModel * vec4(aPos, 1.0);
}