#define NUM_CASCADES 3

uniform sampler2DArray shadowMap;

uniform mat4 lightSpaceMatrix0;
uniform mat4 lightSpaceMatrix1;
uniform mat4 lightSpaceMatrix2;

uniform float cascadeEnds0 = 0;
uniform float cascadeEnds1 = 1;
uniform float cascadeEnds2 = 2;
uniform vec3 shadowCenter = vec3(0.0);

int GetCascadeIndex(float viewDepth);
float ShadowCalculation(vec3 fragPos, int index, mat4 matrix);
float ShadowCalculation2(vec3 fragPos, vec3 normal, vec3 lightDir, int index, mat4 matrix);
float ShadowBlended(vec3 fragPos, vec3 normal, vec3 lightDir, float viewDepth);

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

const vec2 poissonDisk[16] = vec2[](
    vec2(-0.94201624, -0.39906216),
    vec2( 0.94558609, -0.76890725),
    vec2(-0.09418410, -0.92938870),
    vec2( 0.34495938,  0.29387760),
    vec2(-0.91588581,  0.45771432),
    vec2(-0.81544232, -0.87912464),
    vec2(-0.38277543,  0.27676845),
    vec2( 0.97484398,  0.75648379),
    vec2( 0.44323325, -0.97511554),
    vec2( 0.53742981, -0.47373420),
    vec2(-0.26496911, -0.41893023),
    vec2( 0.79197514,  0.19090188),
    vec2(-0.24188840,  0.99706507),
    vec2(-0.81409955,  0.91437590),
    vec2( 0.19984126,  0.78641367),
    vec2( 0.14383161, -0.14100790)
);

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

    float dotNL = max(dot(normal, lightDir), 0.0);
    float bias = max(MIN_BIAS * (1.0 - dotNL), MIN_BIAS);

    vec2 texelSize = 1.0 / vec2(textureSize(shadowMap, 0).xy);
    // Vzdálené kaskády mají větší texely — rozšíříme kernel aby stíny nevypadaly ostřejší v dálce
    float spread = (index == 0) ? 1.0 : (index == 1) ? 2.0 : 3.0;

    float shadow = 0.0;
    float currentDepth = projCoords.z - bias;
    for (int i = 0; i < 16; i++) {
        vec2 offset = poissonDisk[i] * texelSize * spread;
        shadow += texture(shadowMap, vec3(projCoords.xy + offset, index)).r < currentDepth ? 1.0 : 0.0;
    }

    return shadow / 16.0;
}

int GetCascadeIndex(float viewDepth)
{
    if(viewDepth < cascadeEnds0) return 0;
    if(viewDepth < cascadeEnds1) return 1;
    return 2;
}

float ShadowBlended(vec3 fragPos, vec3 normal, vec3 lightDir, float viewDepth)
{
    int  cascadeIndex  = GetCascadeIndex(viewDepth);
    mat4 primaryMatrix = (cascadeIndex == 0) ? lightSpaceMatrix0
                       : (cascadeIndex == 1) ? lightSpaceMatrix1
                       : lightSpaceMatrix2;

    float shadow = ShadowCalculation2(fragPos, normal, lightDir, cascadeIndex, primaryMatrix);

    // Blend last 15 % of each cascade with the next one to hide the hard transition seam
    if (cascadeIndex < 2) {
        float cascadeEnd = (cascadeIndex == 0) ? cascadeEnds0 : cascadeEnds1;
        float blendStart = cascadeEnd * 0.85;

        if (viewDepth > blendStart) {
            float blendFactor = smoothstep(blendStart, cascadeEnd, viewDepth);
            int   nextCascade = cascadeIndex + 1;
            mat4  nextMatrix  = (nextCascade == 1) ? lightSpaceMatrix1 : lightSpaceMatrix2;
            float shadowNext  = ShadowCalculation2(fragPos, normal, lightDir, nextCascade, nextMatrix);
            shadow = mix(shadow, shadowNext, blendFactor);
        }
    }

    return shadow;
}