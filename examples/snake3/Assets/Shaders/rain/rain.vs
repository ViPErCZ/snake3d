#version 330 core
//layout (location = 0) in vec4 vertex; // <vec2 position, vec2 texCoords>
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 3) in vec2 aTexCoords;

out vec2 TexCoords;
out vec4 ParticleColor;
uniform mat4 view;
uniform mat4 model;

uniform mat4 projection;
uniform vec2 offset;
uniform vec4 color;

void main()
{
    float scale = 0.2f;
    TexCoords = aTexCoords;
    ParticleColor = color;
    gl_Position = projection * view * model * vec4((aPos.xy * scale) + offset, 0.0, 1.0);
}