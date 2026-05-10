#version 330 core
out vec4 FragColor;

layout (location = 1) out vec4 BrightColor;

uniform float brightness;
uniform float time;

void main() {
    vec3 boltColor = vec3(0.3, 0.7, 1.0) * 15.0;
    float pulse = sin(time * 30.0) * 0.5 + 0.5;
    vec3 finalColor = boltColor * brightness * (1.0 + pulse * 0.3);
    FragColor = BrightColor = vec4(finalColor, 1.0);
}
