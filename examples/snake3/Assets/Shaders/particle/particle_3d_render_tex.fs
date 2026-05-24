#version 330 core

in vec2 vTex;
in vec4 vColor;
out vec4 FragColor;

uniform sampler2D uTexture0;
uniform float u_colorSensitivity;

void main() {
    vec4 tex = texture(uTexture0, vTex);

    float alpha = tex.a * vColor.a;
    vec3 color = tex.rgb * vColor.rgb * u_colorSensitivity;

    FragColor = vec4(color, alpha);
}