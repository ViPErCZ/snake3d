uniform sampler2D shadowMap;
uniform mat4 lightSpaceMatrix;

float ShadowCalculation(vec3 fragPos);
float ShadowCalculation2(vec3 fragPos, vec3 normal, vec3 lightDir);

float ShadowCalculation(vec3 fragPos)
{
    vec4 fragPosLightSpace = lightSpaceMatrix * vec4(fragPos, 1.0);
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowMap, projCoords.xy).r;
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // check whether current frag pos is in shadow
    float shadow = currentDepth > closestDepth  ? 1.0 : 0.0;

    return shadow;
}

float ShadowCalculation2(vec3 fragPos, vec3 normal, vec3 lightDir)
{
    vec4 fragPosLightSpace = lightSpaceMatrix * vec4(fragPos, 1.0);
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;

    if (projCoords.z > 1.0) {
        return 0.0;
    }

    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowMap, projCoords.xy).r;

    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // check whether current frag pos is in shadow
    float shadow = 0.0;

    float dotNL = dot(normal, lightDir);
    dotNL = max(dotNL, 0.0);
    float bias = 0.005 * (1.0 - dotNL);
    int kernel = 5;
    int samples = (kernel * 2 + 1) * (kernel * 2 + 1);

    // PCF
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for(int x = -kernel; x <= kernel; ++x)
    {
        for(int y = -kernel; y <= kernel; ++y)
        {
            vec2 offset = vec2(x, y) * texelSize;
            float pcfDepth = texture(shadowMap, projCoords.xy + offset).r;
            shadow += (projCoords.z - bias > pcfDepth ? 1.0 : 0.0);
        }
    }
    shadow /= float(samples);

    return shadow;
}