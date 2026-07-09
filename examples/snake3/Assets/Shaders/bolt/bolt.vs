#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 startPos;
uniform vec3 endPos;

void main() {
    vec3 position = mix(startPos, endPos, aPos.x);
    gl_Position = projection * view * vec4(position, 1.0);
}
