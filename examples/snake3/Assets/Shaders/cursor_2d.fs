#version 330 core

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D textureMap;
uniform vec4 tint;
uniform float alpha = 1.0;

void main()
{
    vec4 tex = texture(textureMap, TexCoords);
    if (tex.a < 0.01) {
        discard;
    }
    vec4 color = tex * tint;
    color.a *= alpha;
    FragColor = color;
}
