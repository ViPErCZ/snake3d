#version 330 core

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
} fs_in;

struct Light {
    vec3 Position;
    vec3 Color;
};

uniform vec3 lightColor;

void main()
{
    vec3 color = lightColor; //texture(scene, TexCoords).rgb;
    float brightness = dot(color.rgb, vec3(0.2126, 0.7152, 0.0722));

    if (brightness > 1.0) {
        BrightColor = vec4(color, 1.0);
    } else {
        BrightColor = vec4(0.0);
    }

    FragColor = vec4(color, 1.0);
}