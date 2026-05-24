#version 330 core
out vec4 FragColor;
in vec2 TexCoords;

uniform vec2 quadSize;      // width, height
uniform float borderWidth;
uniform vec3 borderColor;
uniform float radius;

// distance od rounded rect ze středu
float roundedRectDist(vec2 p, vec2 size, float r)
{
    vec2 halfSize = size * 0.5;
    vec2 d = abs(p) - (halfSize - vec2(r));
    return length(max(d,0.0)) - r;
}

float rectDist(vec2 p, vec2 size)
{
    vec2 halfSize = size * 0.5;
    vec2 d = abs(p) - halfSize;
    return max(max(d.x, d.y), 0.0);
}

void main()
{
    // uv ve středu quadu [-width/2, width/2]
    vec2 uv = TexCoords * quadSize - quadSize * 0.5;

    // vnější a vnitřní rounded rect
    float distOuter = roundedRectDist(uv, quadSize, radius);
    //float distInner = roundedRectDist(uv, quadSize - 2.0 * vec2(borderWidth), radius);
    float distInner = rectDist(uv, quadSize - 2.0 * vec2(borderWidth));

    // Discard mimo vnější tvar – odstraní černé trojúhelníky
    if (distOuter > 0.0)
        discard;

    // vykresli jen okraj
    if(distOuter <= 0.0 && distInner > 0.0)
        FragColor = vec4(borderColor, 0.7);
    else
        FragColor = vec4(0.0, 0.0, 0.0, 0.3);
}
