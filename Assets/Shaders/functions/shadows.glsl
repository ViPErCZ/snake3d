#define NUM_CASCADES 3

uniform sampler2DArray shadowMap;

uniform mat4 lightSpaceMatrix0;
uniform mat4 lightSpaceMatrix1;
uniform mat4 lightSpaceMatrix2;

uniform float cascadeEnds0 = 0;
uniform float cascadeEnds1 = 1;
uniform float cascadeEnds2 = 2;

int GetCascadeIndex(float viewDepth);
float ShadowCalculation(vec3 fragPos, int index, mat4 matrix);
float ShadowCalculation2(vec3 fragPos, vec3 normal, vec3 lightDir, int index, mat4 matrix);

float ShadowCalculation(vec3 fragPos, int index, mat4 matrix)
{
    vec4 fragPosLightSpace = matrix * vec4(fragPos, 1.0);
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowMap, vec3(projCoords.xy, index)).r;
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // check whether current frag pos is in shadow
    float shadow = currentDepth > closestDepth  ? 1.0 : 0.0;

    return shadow;
}

float saturate(float val) {
    return clamp(val, 0.0, 1.0);
}

float ShadowCalculation2(vec3 fragPos, vec3 normal, vec3 lightDir, int index, mat4 matrix)
{
    const float MIN_BIAS = 0.0005;
    float cosTheta = saturate(dot(normal, -lightDir));
    float sinTheta = sqrt(1.0 - cosTheta * cosTheta);
    fragPos += normal * (sinTheta * MIN_BIAS);

    vec4 fragPosLightSpace = matrix * vec4(fragPos, 1.0);
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;

    if (projCoords.z > 1.0)
      return 0.0;

    float currentDepth = projCoords.z;

    float dotNL = max(dot(normal, lightDir), 0.0);
    float bias = MIN_BIAS * (1.0 - dotNL);

    int kernel = 1;
    int samples = (kernel * 2 + 1) * (kernel * 2 + 1);
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0).xy;

    float shadow = 0.0;
    float edgeThreshold = 0.0009; // šířka hranového gradientu

    for(int x = -kernel; x <= kernel; ++x)
    {
        for(int y = -kernel; y <= kernel; ++y)
        {
            float pcfDepth = texture(shadowMap, vec3(projCoords.xy + vec2(x, y) * texelSize, index)).r;
            float diff = currentDepth - bias - pcfDepth;

            // gradient jen na hraně
            float edgeFactor = smoothstep(0.0, edgeThreshold, diff);
            shadow += diff > 0.0 ? edgeFactor : 0.0;
        }
    }

    shadow /= float(samples);

    return shadow; // 0 = osvětleno, 1 = ve stínu, gradient jen na hraně
}

int GetCascadeIndex(float viewDepth)
{
    if(viewDepth < cascadeEnds0) {
        return 0;
    }

    if(viewDepth < cascadeEnds1) {
        return 1;
    }

    return 2;
}