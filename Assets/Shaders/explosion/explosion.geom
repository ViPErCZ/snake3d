#version 450

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

in vec3 fColor[];
in vec3 vPos[];
in vec3 vNormal[];
in vec2 vTex[];
out vec4 vColor;
out vec3 fragNormal;
out vec3 fragPos;
out vec2 TexCoords;

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

    // Výpočet normály plochy (trojúhelníku) pro směr odletu celé části
    vec3 v1 = vPos[1] - vPos[0];
    vec3 v2 = vPos[2] - vPos[0];
    vec3 faceNormal = normalize(cross(v1, v2));

    // Přidáme trochu náhodnosti do směru založené na pozici středu trojúhelníku
    vec3 center = (vPos[0] + vPos[1] + vPos[2]) / 3.0;
    vec3 dir = normalize(faceNormal * 0.7 + randDir(center) * 0.3);

    // Mírný jitter délky života, aby střepy nepůsobily úplně synchronně.
    // Úzký rozsah omezuje viditelné rozdíly velikostí mezi střepy.
    float lifeJitter = mix(0.99, 1.0, hash(center + vec3(3.17, 9.31, 1.73)));
    float localT = clamp(t / lifeJitter, 0.0, 1.0);

    // Kratší a mírně pomalejší rozlet od středu.
    float moveFactor = explosionRadius * smoothstep(0.0, 1.0, pow(localT, 1.2));
    
    // Zmenšování střepů v čase
    float scale = 1.0 - smoothstep(0.60, 1.0, localT);

    // Kompenzace velikosti podle velikosti původního trojúhelníku:
    // velké trojúhelníky se při explozi stáhnou víc, malé necháme beze změny.
    float edge01 = length(v1);
    float edge02 = length(v2);
    float edge12 = length(vPos[2] - vPos[1]);
    float avgEdge = (edge01 + edge02 + edge12) / 3.0;
    float sizeCompensation = clamp(0.08 / max(avgEdge, 0.0001), 0.28, 0.90);

    for(int i = 0; i < 3; i++)
    {
        // Posuneme každý vrchol o stejný vektor, aby se trojúhelník neroztrhal, ale odletěl jako celek
        // A aplikujeme škálování kolem středu trojúhelníku
        vec3 vertexPos = vPos[i];
        vec3 centeredPos = (vertexPos - center) * sizeCompensation;
        vec3 displaced = center + (centeredPos * scale) + (dir * moveFactor);

        // Fade-out přes alpha kanál: drží se déle viditelný a pak rychleji mizí
        float alpha = 1.0 - smoothstep(0.18, 1.0, localT);
        alpha *= alpha;

        vColor = vec4(fColor[i], alpha);
        fragNormal = normalize(vNormal[i]);
        fragPos = displaced;
        TexCoords = vTex[i];

        gl_Position = projection * view * vec4(displaced, 1.0);
        EmitVertex();
    }
    EndPrimitive();
}
