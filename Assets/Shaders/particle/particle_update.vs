#version 330 core

layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inVel;
layout(location = 2) in float inLife;
layout(location = 3) in float inSeed;

uniform float u_dt;
uniform vec3  u_emitterPos;

out mat4 outModel;
out vec4 outColor;

float rand(float n) {
    return fract(sin(n) * 43758.5453123);
}

mat4 compose(vec3 pos, vec3 scale) {
    return mat4(
        scale.x, 0, 0, 0,
        0, scale.y, 0, 0,
        0, 0, scale.z, 0,
        pos.x, pos.y, pos.z, 1
    );
}

void main() {
    vec3 pos = inPos;
    vec3 vel = inVel;
    float life = inLife;

    if (life <= 0.0) {
        float r1 = rand(inSeed + u_dt);
        float r2 = rand(inSeed * 2.3);

        pos = u_emitterPos + vec3(
            (r1 - 0.5) * 0.1,
            0.0,
            (r2 - 0.5) * 0.1
        );

        vel = vec3(
            (r1 - 0.5) * 0.2,
            rand(inSeed) * 1.5,
            (r2 - 0.5) * 0.2
        );

        life = 1.5;
    } else {
        life -= u_dt;
        pos += vel * u_dt;
    }

    float t = clamp(life / 1.5, 0.0, 1.0);
    vec3 scale = mix(vec3(0.02), vec3(0.08), t);

    outModel = compose(pos, scale);
    outColor = vec4(6.0, 3.0, 1.0, t);

    gl_Position = vec4(0.0);
}