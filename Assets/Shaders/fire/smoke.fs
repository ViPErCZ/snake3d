#version 330 core

in vec4 vColor;

out vec4 FragColor;

uniform sampler2D uTexture;

void main() {
    vec4 texColor = texture(uTexture, gl_PointCoord);
    if (texColor.a < 0.2) discard;
    FragColor = vec4(texColor.rgb * vColor.rgb, texColor.a * vColor.a * 0.7);
}