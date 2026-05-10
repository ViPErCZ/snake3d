#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 3) in vec2 aTex;

layout(location = 4) in vec3 iPos;
layout(location = 5) in vec3 iVel;
layout(location = 6) in float iLife;
layout(location = 7) in float iSeed;

out vec2 vTex;
out vec4 vColor;

uniform mat4 view;
uniform mat4 projection;
uniform mat4 model;

uniform float u_lifeMax;
uniform float u_sizeMin;
uniform float u_sizeMax;
uniform vec4  u_colorStart;
uniform vec4  u_colorEnd;
uniform float u_stretch;
uniform int   u_mode; // 0 = Billboard, 1 = Rain/Stretched

mat3 rotateAxis(vec3 axis, float angle) {
    axis = normalize(axis);
    float s = sin(angle);
    float c = cos(angle);
    float oc = 1.0 - c;
    return mat3(
    oc * axis.x * axis.x + c,           oc * axis.x * axis.y - axis.z * s,  oc * axis.z * axis.x + axis.y * s,
    oc * axis.x * axis.y + axis.z * s,  oc * axis.y * axis.y + c,           oc * axis.y * axis.z - axis.x * s,
    oc * axis.z * axis.x - axis.y * s,  oc * axis.y * axis.z + axis.x * s,  oc * axis.z * axis.z + c
    );
}

void main() {
    vTex = aTex;

    float t = clamp(iLife / max(u_lifeMax, 0.0001), 0.0, 1.0);
    float s;
    if (u_mode == 1) {
        float variation = fract(sin(iSeed) * 43758.5453);
        s = mix(u_sizeMin, u_sizeMax, variation);
    } else {
        s = mix(u_sizeMin, u_sizeMax, t);
    }
    vColor = mix(u_colorEnd, u_colorStart, t);

    vec3 camRight = vec3(view[0][0], view[1][0], view[2][0]);
    vec3 camUp    = vec3(view[0][1], view[1][1], view[2][1]);

    vec3 finalWorldPos;

    if (u_mode == 1) {
        // --- RAIN / STRETCHED ---
        vec3 stretchDir = normalize(iVel);
        float speed = length(iVel);
        vec3 offset = (camRight * aPos.x * s) + (stretchDir * aPos.z * (s + speed * u_stretch));
        finalWorldPos = (model * vec4(iPos + offset, 1.0)).xyz;
    }
    else if (u_mode == 2) {
        // --- MESH / CUBES ---
        vec3 randomAxis = normalize(vec3(sin(iSeed), cos(iSeed * 1.5), sin(iSeed * 3.2)));
        float angle = iSeed * 10.0 + (1.0 - t) * 10.0;
        mat3 rotation = rotateAxis(randomAxis, angle);

        vec3 localPos = rotation * (aPos * s);
        finalWorldPos = (model * vec4(iPos + localPos, 1.0)).xyz;
    }
    else {
        // --- BILLBOARD (Exploze, Oheň) ---
        vec3 worldCenter = (model * vec4(iPos, 1.0)).xyz;

        float sx = s;
        float sy = s + length(iVel) * u_stretch;

        vec3 offset = (camRight * aPos.x * sx) + (camUp * aPos.z * sy);
        finalWorldPos = worldCenter + offset;
    }

    gl_Position = projection * view * vec4(finalWorldPos, 1.0);
}