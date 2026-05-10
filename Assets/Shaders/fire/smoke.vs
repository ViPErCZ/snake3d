#version 330 core

layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoords;
layout (location = 2) in vec3 instancePosition;
layout (location = 3) in vec4 instanceColor;
// Přidáme další atributy pro každou instanci
layout (location = 4) in vec2 sizeAndRotation; // x = velikost, y = rotace v radiánech

out vec2 TexCoords;
out vec4 ParticleColor;

uniform mat4 view;
uniform mat4 projection;
uniform float overallSize;

void main() {
    TexCoords = aTexCoords;
    ParticleColor = instanceColor;

    float particleSize = sizeAndRotation.x * overallSize;
    float particleRotation = sizeAndRotation.y;

    // Rotační matice pro 2D rotaci billboardu
    mat2 rotMatrix = mat2(
        cos(particleRotation), -sin(particleRotation),
        sin(particleRotation),  cos(particleRotation)
    );
    vec2 rotatedPos = rotMatrix * aPos;

    vec3 cameraRight = vec3(view[0][0], view[1][0], view[2][0]);
    vec3 cameraUp = vec3(view[0][1], view[1][1], view[2][1]);

    vec3 pos = instancePosition + (cameraRight * rotatedPos.x + cameraUp * rotatedPos.y) * particleSize;

    gl_Position = projection * view * vec4(pos, 1.0);
}