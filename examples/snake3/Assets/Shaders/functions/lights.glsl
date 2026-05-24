struct Material {
    sampler2D ambient;
    sampler2D diffuse;
    sampler2D specular;
    sampler2D aoMap;
    float shininess;
};

struct DirLight {
    vec3 position;
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct MaterialDirLight {
    vec3 direction;

    vec3 diffuse;
    vec3 specular;
};

struct PointLight {
    vec3 position;

    float constant;
    float linear;
    float quadratic;
    //float energy;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct SpotLight {
    vec3 position;
    vec3 direction;
    float cutOff;
    float outerCutOff;

    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    bool pulse;
};

#define NR_POINT_LIGHTS 8

uniform DirLight dirLight;
uniform MaterialDirLight materialDirLight;
uniform PointLight pointLight[NR_POINT_LIGHTS];
uniform SpotLight spotLight[NR_POINT_LIGHTS];
uniform Material material;
uniform int numPointLights = 0;
uniform int numSpotLights = 0;
uniform bool useMaterial = false;
uniform bool normalMapEnabled = false;
uniform bool specularMapEnabled = false;
uniform bool hasAlbedoTexture = false;
uniform samplerCube environmentMap;
uniform bool iblEnabled = false;
uniform float uShadowAmbientDarken = 0.85; // how much to darken ambient in shadow (0..1)
uniform float uShadowDesaturateStrength = 1.0; // how strong the gray shift is in shadow (0..1)

// function prototypes
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir, vec3 ambientColor, float shadow);
vec3 CalcDirLightMaterial(MaterialDirLight light, vec3 normal, vec3 viewDir, vec3 fragPos, vec3 ambient);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 materialColor);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 materialColor, float timer, vec3 albedoColor, vec3 specularColor);
#ifdef FEATURE_PBR
vec3 CalcDirLightPBR(DirLight light, vec3 fragPos, vec3 normal, vec3 viewDir, vec3 ambientColor, float roughness, float metalness, vec3 F0);
vec3 CalcPointLightPBR(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 albedo, float roughness, float metalness, vec3 F0);
vec3 CalcSpotLightPBR(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir,
                      float timer, vec3 albedo, float roughness, float metalness, vec3 F0);
vec3 fresnelSchlick(float cosTheta, vec3 F0);
float DistributionGGX(vec3 N, vec3 H, float roughness);
float GeometrySchlickGGX(float NdotV, float roughness);
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness);
#endif
#ifdef FEATURE_IBL
vec3 CalcIBLSpecular(vec3 R, float roughness, vec3 F0);
vec3 CalcIBLDiffuse(vec3 N);
#endif

// calculates the color when using a directional light.
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir, vec3 ambientColor, float shadow)
{
    normal = normalize(normal);
    viewDir = normalize(viewDir);

    vec3 lightDir = normalize(-light.direction);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // combine results
    vec3 ambient = ambientColor;
    if (useMaterial == false) {
        ambient = vec3(texture(material.ambient, TexCoords));
    }

    vec3 diffuse = light.diffuse * diff;
    if (normalMapEnabled) {
        diffuse *= vec3(texture(material.diffuse, TexCoords));
    }
    vec3 specular = light.specular * spec;
    if (specularMapEnabled) {
        specular *= vec3(texture(material.specular, TexCoords));
    }

    // Darken and desaturate ambient in shadow so cores are darker and edges go through gray
    vec3 ambientLit = ambient * light.ambient;
    float shadowAmount = clamp(shadow, 0.0, 1.0);

    // 1) darken toward black based on shadow
    vec3 ambientDark = mix(ambientLit, vec3(0.0), shadowAmount * clamp(uShadowAmbientDarken, 0.0, 1.0));

    // 2) desaturate toward gray based on shadow
    float luminance = dot(ambientDark, vec3(0.299, 0.587, 0.114));
    vec3 ambientGray = vec3(luminance);
    vec3 ambientAdjusted = mix(ambientDark, ambientGray, shadowAmount * clamp(uShadowDesaturateStrength, 0.0, 1.0));

    return ambientAdjusted + (diffuse + specular) * (1.0 - shadow);
}

#ifdef FEATURE_PBR
vec3 CalcDirLightPBR(
    DirLight light,
    vec3 normal,
    vec3 fragPos,
    vec3 viewDir,
    vec3 ambientColor,
    float roughness,
    float metalness,
    vec3 F0
    ) {
    vec3 N = normalize(normal);
    vec3 V = normalize(viewDir);
    vec3 L = normalize(-light.direction);
    vec3 H = normalize(V + L);

    float NdotL = max(dot(N, L), 0.0);

    // PBR Cook-Torrance
    float NDF = DistributionGGX(N, H, roughness);
    float G   = GeometrySmith(N, V, L, roughness);
    vec3  F   = fresnelSchlick(max(dot(H, V), 0.0), F0);

    vec3 numerator    = NDF * G * F;
    float denominator = 4.0 * max(dot(N, V), 0.0) * NdotL + 0.0001;
    vec3 specular = numerator / denominator;

    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= 1.0 - metalness;

    vec3 albedo = texture(material.ambient, TexCoords).rgb;

    // Výsledek je jen Diffuse + Specular od SLUNCE
    return (kD * albedo / 3.14159265 + specular) * light.diffuse * NdotL;
}
#endif

vec3 CalcDirLightMaterial(MaterialDirLight light, vec3 normal, vec3 viewDir, vec3 fragPos, vec3 ambient)
{
    vec3 lightDir = normalize(light.direction - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // combine results
    vec3 diffuse = light.diffuse * diff;
    vec3 specular = light.specular * spec;

    return (ambient + diffuse + specular);
}

// calculates the color when using a point light.
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 materialColor, vec3 albedoColor, vec3 specularColor)
{
    vec3 N = normalize(normal);
    vec3 lightDir = normalize(light.position - fragPos);
    float diff = max(dot(N, lightDir), 0.0);

    vec3 reflectDir = reflect(-lightDir, N);
    float spec = pow(max(dot(normalize(viewDir), reflectDir), 0.0), material.shininess);

    float distance = length(light.position - fragPos);
    distance = max(distance, 0.1);
    float ld = (light.constant + light.linear * distance + light.quadratic * distance * distance);
    ld = max(ld, 0.1);
    float attenuation = 1.0 / ld;

    vec3 albedo = hasAlbedoTexture ? albedoColor : materialColor;
    vec3 ambient = light.ambient * albedo;
    vec3 diffuse = light.diffuse * diff * albedo;
    vec3 specular = useMaterial ? light.specular * spec : light.specular * spec * specularColor;

    return (ambient + diffuse + specular) * attenuation;
}

#ifdef FEATURE_PBR
vec3 CalcPointLightPBR(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 albedo, float roughness, float metalness, vec3 F0)
{
    roughness = max(roughness, 0.05);

    vec3 L = normalize(light.position - fragPos);
    vec3 H = normalize(viewDir + L);

    // Ochrana distance (proti crashi)
    float distance = length(light.position - fragPos);
    distance = max(distance, 0.05);
    float ld = (light.constant + light.linear * distance + light.quadratic * distance * distance);
    ld = max(ld, 0.05);

    float attenuation = 1.0 / ld;
    vec3 radiance = light.diffuse * attenuation;

    float NDF = DistributionGGX(normal, H, roughness);
    float G   = GeometrySmith(normal, viewDir, L, roughness);
    vec3 F    = fresnelSchlick(max(dot(H, viewDir), 0.0), F0);

    vec3 numerator    = NDF * G * F;
    float denominator = 4.0 * max(dot(normal, viewDir), 0.0) * max(dot(normal, L), 0.0) + 0.0001;
    vec3 specular = numerator / denominator;

    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= 1.0 - metalness;

    float NdotL = max(dot(normal, L), 0.001);

    return (kD * albedo / 3.14159265 + specular) * radiance * NdotL;
}
#endif

float computePulse(float timer)
{
    // ==== Pulzování ====
    float pulse = 0.7f
    + 0.1f * sin(timer * 0.7f)
    + 0.05f * sin(timer * 1.3f + 1.1f)
    + 0.03f * sin(timer * 2.1f + 2.4f);

    // Výsledná intenzita mezi 0.6 – 0.9, s velmi plynulými změnami
    return clamp(pulse, 0.6f, 0.9f);
}

// calculates the color when using a spot light.
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 materialColor, float timer, vec3 albedoColor, vec3 specularColor)
{
    float currentCutOff = light.cutOff;
    float currentOuterCutOff = light.outerCutOff;

    if (light.pulse) {
        float baseAngle = acos(light.cutOff);
        float baseOuterAngle = acos(light.outerCutOff);
        float angleDelta = radians(0.5) * sin(timer * 3.5);
        currentCutOff = cos(baseAngle + angleDelta);
        currentOuterCutOff = cos(baseOuterAngle + angleDelta);
    }
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    // spotlight intensity
    float theta = dot(lightDir, normalize(-light.direction));

    //float offset = 0.03 * sin(timer * 2.3 + fragPos.x * 10.0) + 0.02 * sin(timer * 1.7 + fragPos.y * 12.0);

    float epsilon = currentCutOff - currentOuterCutOff;
    float intensity = clamp((theta - currentOuterCutOff) / epsilon, 0.0, 1.0);
    // combine results
    vec3 ambient = light.ambient * (hasAlbedoTexture ? albedoColor : materialColor);
    vec3 diffuse = light.diffuse * diff;
    vec3 specular = light.specular * spec;
    if (specularMapEnabled) {
        specular = specular * specularColor;
    }
    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;

    if (light.pulse) {
        float pulse = computePulse(timer);
        ambient *= pulse;
        diffuse *= pulse;
    }

    return (ambient + diffuse + specular);
}

#ifdef FEATURE_PBR
vec3 CalcSpotLightPBR(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir,
                      float timer, vec3 albedo, float roughness, float metalness, vec3 F0)
{
    roughness = max(roughness, 0.05);

    // Cone falloff with optional pulse-driven angle wobble (mirrors
    // CalcSpotLight's behaviour so a pulsing lamp looks the same on PBR
    // materials as on Phong ones).
    float currentCutOff = light.cutOff;
    float currentOuterCutOff = light.outerCutOff;
    if (light.pulse) {
        float baseAngle = acos(light.cutOff);
        float baseOuterAngle = acos(light.outerCutOff);
        float angleDelta = radians(0.5) * sin(timer * 3.5);
        currentCutOff = cos(baseAngle + angleDelta);
        currentOuterCutOff = cos(baseOuterAngle + angleDelta);
    }

    vec3 L = normalize(light.position - fragPos);
    float theta = dot(L, normalize(-light.direction));
    float epsilon = currentCutOff - currentOuterCutOff;
    float intensity = clamp((theta - currentOuterCutOff) / epsilon, 0.0, 1.0);
    if (intensity <= 0.0) {
        return vec3(0.0);
    }

    // Distance attenuation (same form as point light PBR).
    float distance = length(light.position - fragPos);
    distance = max(distance, 0.05);
    float ld = (light.constant + light.linear * distance + light.quadratic * distance * distance);
    ld = max(ld, 0.05);
    float attenuation = 1.0 / ld;

    vec3 radiance = light.diffuse * attenuation * intensity;

    vec3 H = normalize(viewDir + L);
    float NDF = DistributionGGX(normal, H, roughness);
    float G   = GeometrySmith(normal, viewDir, L, roughness);
    vec3 F    = fresnelSchlick(max(dot(H, viewDir), 0.0), F0);

    vec3 numerator    = NDF * G * F;
    float denominator = 4.0 * max(dot(normal, viewDir), 0.0) * max(dot(normal, L), 0.0) + 0.0001;
    vec3 specular = numerator / denominator;

    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= 1.0 - metalness;

    float NdotL = max(dot(normal, L), 0.001);
    vec3 contribution = (kD * albedo / 3.14159265 + specular) * radiance * NdotL;

    if (light.pulse) {
        contribution *= computePulse(timer);
    }
    return contribution;
}
#endif

#ifdef FEATURE_PBR
vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = 3.14159265 * denom * denom;
    return nom / max(denom, 0.000001);
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;
    return nom / max(denom, 0.000001); // Ochrana
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}
#endif

#ifdef FEATURE_IBL
vec3 CalcIBLSpecular(vec3 R, float roughness, vec3 F0) {
    vec3 prefilteredColor;
    if (iblEnabled) {
        return textureLod(environmentMap, R, roughness * 4.0).rgb;
    }  else {
        prefilteredColor = F0 * vec3(0) * 0.3;
    }
    return F0 * prefilteredColor;
}

vec3 CalcIBLDiffuse(vec3 N) {
    // difuzní irradiance z environment mapy
    vec3 irradiance = texture(environmentMap, N).rgb;
    return irradiance;
}
#endif