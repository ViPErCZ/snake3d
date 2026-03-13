#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexUV;

out vec2 uv;
uniform vec2 iResolution;

void main() {
    gl_Position = vec4(aPos.x / (iResolution.x / 2.0), aPos.y / (iResolution.y / 2.0), 0.0, 1.0);
    uv = aTexUV * 2.0 - 1.0;
}
