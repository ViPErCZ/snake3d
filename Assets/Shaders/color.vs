#version 330 core

layout(location=0) in vec2 aQuad;

uniform vec3 ringCenter;
uniform mat4 view;
uniform mat4 proj;
uniform float radius;
uniform float pulse;

out vec2 vQuad;

void main() {
    vQuad = aQuad;
    float scale = radius * (1.0 + 0.1 * pulse);
    vec3 offset = vec3(aQuad * scale, 0.0); // kruh v rovině XY, Z-up
    vec4 worldPos = vec4(ringCenter + offset, 1.0);
    gl_Position = proj * view * worldPos;
}