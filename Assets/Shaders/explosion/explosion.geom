#version 450

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

in vec3 fColor[];
in vec3 vPos[];
in vec3 vNormal[];
out vec4 vColor;

uniform mat4 view;
uniform mat4 projection;

uniform float time;           // čas od startu exploze
uniform float explosionRadius; // maximální rozlet (dřív "speed")
uniform float fadeTime;       // délka celé animace

float hash(vec3 p) {
    return fract(sin(dot(p ,vec3(12.9898,78.233, 45.164))) * 43758.5453);
}

vec3 randDir(vec3 pos)
{
    return normalize(vec3(
        hash(pos.xyz + 1.23),
        hash(pos.yzx + 4.56),
        hash(pos.zxy + 7.89)
    ) * 2.0 - 1.0);
}

void main()
{
    // normalizovaný čas 0..1
    float t = clamp(time / fadeTime, 0.0, 1.0);

    // křivka rozletu (rychle na začátku, pak zpomaluje)
    float moveFactor = smoothstep(0.0, 1.0, t) * explosionRadius;

    for(int i = 0; i < 3; i++)
    {
        vec3 dir = normalize(vNormal[i] * 0.6 + randDir(vPos[i]) * 0.4);

        // konečná pozice – nikdy dál než explosionRadius
        vec3 displaced = vPos[i] + dir * moveFactor;

        float alpha = 1.0 - t;

        //float randVal = hash(vPos[i]);
        //vec3 baseColor = mix(vec3(1.0, 0.3, 0.0), vec3(1.0, 0.7, 0.2), randVal);

        //fColor = vec4(baseColor, alpha);
        vColor = vec4(fColor[i], alpha);

        gl_Position = projection * view * vec4(displaced, 1.0);
        EmitVertex();
    }
    EndPrimitive();
}
