#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 norm;
layout (location = 2) in vec2 texCoord;

out vec3 normal;
out vec2 TexCoord;

void main()
{
    gl_Position = vec4(position, 2.0f);
    TexCoord = texCoord;
    normal = norm;
}
