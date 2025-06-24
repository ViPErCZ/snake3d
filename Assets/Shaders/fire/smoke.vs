#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec4 aColor;
layout(location = 2) in float aSize;

out vec4 vColor;
uniform mat4 uViewProj;

void main() {
    gl_Position = uViewProj * vec4(aPos,1.0);
    float life = aColor.a;
    if (aSize < 0.1) {
        gl_PointSize = 20.0;
    } else {
        gl_PointSize = aSize;
    }

    vColor = aColor;
}