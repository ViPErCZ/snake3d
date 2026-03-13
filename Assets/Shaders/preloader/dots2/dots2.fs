#version 330 core
out vec4 FragColor;
in vec2 uv;

uniform float iTime;
uniform vec2 iResolution;

#define PI 3.14159265

void main() {
    float time = iTime;
    float mx = max(iResolution.x, iResolution.y);
    vec2 scrs = iResolution.xy / mx;

    // uv is -1..1, convert to 0..1 and scale to screen space
    vec2 uvn = (uv * 0.5 + 0.5) * scrs;

    vec3 col = vec3(0.0);
    float x = 0.0;
    float y = 0.0;
    float radius = 0.02;
    const float dotsnb = 10.0;

    for (float i = 0.0; i < dotsnb; i++) {
        x = 0.1 * cos(2.0 * PI * i / dotsnb + time * (i + 3.0) / 3.0);
        y = 0.1 * sin(2.0 * PI * i / dotsnb + time * (i + 3.0) / 3.0);

        float dist = distance(uvn, scrs / 2.0 + vec2(x, y));
        float d = 1.0 - smoothstep(radius - 0.01, radius, dist);

        col += vec3(
            d * (sin(i / dotsnb + time + 2.0 * PI / 3.0) + 1.0) / 2.0,
            d * (sin(i / dotsnb + time + 4.0 * PI / 3.0) + 1.0) / 2.0,
            d * (sin(i / dotsnb + time + 6.0 * PI / 3.0) + 1.0) / 2.0
        );
    }

    col = clamp(col, 0.0, 1.0);
    float intensity = clamp(max(col.r, max(col.g, col.b)), 0.0, 1.0);
    col *= 0.8;
    float alpha = 0.60 + 0.30 * intensity;

    FragColor = vec4(col, alpha);
}
