#version 330 core

layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoords;
layout (location = 2) in vec3 instancePosition;
layout (location = 3) in vec4 instanceColor;

out vec2 TexCoords;
out vec4 ParticleColor;

uniform mat4 view;
uniform mat4 projection;
uniform float particleSize;

void main() {
    TexCoords = aTexCoords;
    ParticleColor = instanceColor;

    vec3 cameraRight = vec3(view[0][0], view[1][0], view[2][0]);
    vec3 cameraUp = vec3(view[0][1], view[1][1], view[2][1]);

    // Použijeme uniform proměnnou pro velikost
    vec3 pos = instancePosition + cameraRight * aPos.x * particleSize + cameraUp * aPos.y * particleSize;

    gl_Position = projection * view * vec4(pos, 1.0);
}