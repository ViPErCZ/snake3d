#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D texture_diffuse1;
uniform vec3 Color;
uniform bool useMaterial = false;

void main()
{
    if (useMaterial) {
        FragColor = vec4(Color, 0);
    } else {
        FragColor = texture(texture_diffuse1, TexCoords);
    }
}