uniform sampler2D shadowMap;
uniform mat4 lightSpaceMatrix;

//float ShadowCalculation(vec4 fragPosLightSpace, sampler2D shadowMap, sampler2D normalMap)
//{
//    // perform perspective divide
//    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
//    // transform to [0,1] range
//    projCoords = projCoords * 0.5 + 0.5;
//    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
//    float closestDepth = texture(shadowMap, projCoords.xy).r;
//    // get depth of current fragment from light's perspective
//    float currentDepth = projCoords.z;
//    const float MIN_BIAS = 0.0005;
//    vec3 normal = texture(normalMap, fs_in.TexCoords).rgb;
//    float bias = max(0.05 * (1.0 - dot(normal, vec3(-fragPosLightSpace.xyz))), MIN_BIAS);
//    // check whether current frag pos is in shadow
//    float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;
//
//    return shadow;
//}

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