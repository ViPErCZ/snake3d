#version 330 core

layout (location = 0) in vec3 aPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float radius;
uniform float pulse;

out vec2 vQuad;

void main() {
    vQuad = aPos.xz * 2.0;

    vec3 worldOrigin = vec3(model[3][0], model[3][1], model[3][2]);
    float currentRadius = radius * (1.0 + 0.1 * pulse);
    vec3 worldPos = worldOrigin + vec3(aPos.x * 2.0 * currentRadius, aPos.z * 2.0 * currentRadius, 0.0);

    gl_Position = projection * view * vec4(worldPos, 1.0);
}