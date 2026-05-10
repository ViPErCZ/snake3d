// functions/rainRipple.glsl

#define HASHSCALE1 .1031
#define HASHSCALE3 vec3(.1031, .1030, .0973)

float hash12(vec2 p) {
    vec3 p3  = fract(vec3(p.xyx) * HASHSCALE1);
    p3 += dot(p3, p3.yzx + 19.19);
    return fract((p3.x + p3.y) * p3.z);
}

vec2 hash22(vec2 p) {
    vec3 p3 = fract(vec3(p.xyx) * HASHSCALE3);
    p3 += dot(p3, p3.yzx+19.19);
    return fract((p3.xx+p3.yz)*p3.zy);
}

vec2 getRainRippleDistortion(vec2 uv, float time, float speed, float density) {
    vec2 p_uv = uv * density;
    vec2 p0 = floor(p_uv);
    vec2 circles = vec2(0.);
    int MAX_RADIUS = 1;

    for (int j = -MAX_RADIUS; j <= MAX_RADIUS; ++j) {
        for (int i = -MAX_RADIUS; i <= MAX_RADIUS; ++i) {
            vec2 pi = p0 + vec2(i, j);

            // Použijeme hash k rozhodnutí, zda v této buňce vůbec prší
            // Změň 0.3 na nižší číslo (např. 0.1) pro ještě méně kapek
            if (hash12(pi + 42.0) > 0.3) continue;

            vec2 p = pi + hash22(pi);
            float t = fract(speed * time + hash12(pi));

            vec2 v = p - p_uv;
            float dist = length(v);
            float wavePos = dist - t * 2.0;

            float ripple = sin(20.0 * wavePos) * smoothstep(-0.5, 0.0, wavePos) * smoothstep(0.5, 0.0, wavePos);
            float s = ripple * (1.0 - t);

            circles += normalize(v) * s;
        }
    }
    return circles;
}