#version 330 core
in vec4 vColor;
out vec4 FragColor;

uniform sampler2D uTexture;

void main() {
    vec4 tex = texture(uTexture, gl_PointCoord);
    FragColor = tex * vColor;
}