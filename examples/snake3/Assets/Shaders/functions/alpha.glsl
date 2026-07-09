uniform float alpha = 1.0;

vec4 alphaBlending(vec3 color)
{
    return vec4(color, alpha);
}