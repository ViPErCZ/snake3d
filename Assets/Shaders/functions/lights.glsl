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
    float energy;

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
};

#define NR_POINT_LIGHTS 16

uniform DirLight dirLight;
uniform MaterialDirLight materialDirLight;
uniform PointLight pointLight[NR_POINT_LIGHTS];
uniform SpotLight spotLight[NR_POINT_LIGHTS];
uniform Material material;
uniform float uTime = 1;
uniform int numPointLights = 0;
uniform int numSpotLights = 0;
uniform bool useMaterial = false;
uniform bool normalMapEnabled = false;
uniform bool specularMapEnabled = false;
uniform samplerCube environmentMap;
uniform bool iblEnabled = false;

// function prototypes
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir, vec3 ambientColor);
vec3 CalcDirLightPBR(DirLight light, vec3 fragPos, vec3 normal, vec3 viewDir, vec3 ambientColor, float roughness, float metalness, vec3 F0);
vec3 CalcDirLightMaterial(MaterialDirLight light, vec3 normal, vec3 viewDir, vec3 fragPos, vec3 ambient);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcPointLightPBR(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, float roughness, float metalness, vec3 F0);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcSpotLightPBR(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 fresnelSchlick(float cosTheta, vec3 F0);
float DistributionGGX(vec3 N, vec3 H, float roughness);
float GeometrySchlickGGX(float NdotV, float roughness);
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness);
vec3 CalcIBLSpecular(vec3 R, float roughness, vec3 F0);
vec3 CalcIBLDiffuse(vec3 N);

// calculates the color when using a directional light.
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir, vec3 ambientColor)
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
        ambient *= vec3(texture(material.ambient, TexCoords));
    }
    vec3 diffuse = light.diffuse * diff;
//    if (normalMapEnabled) {
//        diffuse *= vec3(texture(material.diffuse, TexCoords));
//    }
    vec3 specular = light.specular * spec;
    if (specularMapEnabled) {
        specular *= vec3(texture(material.specular, TexCoords));
    }

    return (ambient + diffuse + specular);
}

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
    // normalizace
    vec3 N = normalize(normal);
    vec3 V = normalize(viewDir);
    vec3 L = normalize(light.position - fragPos);
    vec3 H = normalize(V + L);

    float NdotL = max(dot(N, L), 0.0);

    // PBR výpočty
    float NDF = DistributionGGX(N, H, roughness);
    float G   = GeometrySmith(N, V, L, roughness);
    vec3  F   = fresnelSchlick(max(dot(H, V), 0.0), F0);

    vec3 albedo = texture(material.ambient, TexCoords).rgb;

    vec3 kS = F;
    vec3 kD = (vec3(1.0) - kS) * (1.0 - metalness);

    vec3 diffuse = kD * albedo / 3.14159265;
    vec3 specular = (NDF * G * F) / max(4.0 * max(dot(N, V), 0.0) * NdotL, 0.001);

    // ambientní složka + fallback spekulár pro kov
    vec3 ambient = ambientColor * albedo;
    ambient += F0 * metalness * 0.5; // fallback: kov vždy trochu odráží, i bez IBL
    float ao = texture(material.aoMap, TexCoords).r;
    ambient *= ao;

    // přímé světlo
    vec3 color = ambient + (diffuse + specular) * light.diffuse * NdotL;

    // IBL, pokud je povoleno a environment mapy jsou dostupné
    if (iblEnabled) {
        vec3 R = reflect(-V, N);
        vec3 iblDiffuse  = CalcIBLDiffuse(N) * kD * albedo;
        vec3 iblSpecular = CalcIBLSpecular(R, roughness, F0);
        color += iblDiffuse + iblSpecular;
    }

    return color;
}

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
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 N = normalize(normal);
    vec3 lightDir = normalize(light.position - fragPos);
    float diff = max(dot(N, lightDir), 0.0);

    vec3 reflectDir = reflect(-lightDir, N);
    float spec = pow(max(dot(normalize(viewDir), reflectDir), 0.0), material.shininess);

    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * distance * distance);

    vec3 ambient = useMaterial ? light.ambient * 0.1 : light.ambient * vec3(texture(material.ambient, TexCoords)) * 0.1;
    vec3 diffuse = light.diffuse * diff; // * vec3(texture(material.diffuse, TexCoords));
    vec3 specular = useMaterial ? light.specular * spec : light.specular * spec * vec3(texture(material.specular, TexCoords));

    return (ambient + diffuse + specular) * attenuation;
}

vec3 CalcPointLightPBR(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, float roughness, float metalness, vec3 F0)
{
    vec3 N = normalize(normal);
    vec3 V = normalize(viewDir);
    vec3 L = normalize(light.position - fragPos);
    vec3 H = normalize(V + L);

    float NdotL = max(dot(N, L), 0.0);

    // PBR Fresnel, NDF, Geometry
    float NDF = DistributionGGX(N, H, roughness);
    float G   = GeometrySmith(N, V, L, roughness);
    vec3 F    = fresnelSchlick(max(dot(N, V), 0.0), F0);

    // kS/kD
    vec3 kS = F;
    vec3 kD = (vec3(1.0) - kS) * (1.0 - metalness);

    // Albedo
    vec3 albedo = texture(material.ambient, TexCoords).rgb;

    // Difuse a specular
    vec3 diffuse  = kD * albedo / 3.14159265;
    vec3 specular = (NDF * G * F) / max(4.0 * max(dot(N, V), 0.0) * NdotL, 0.001) + F0 * metalness * 0.1;

    // Attenuace
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * distance * distance);

    vec3 ambient = albedo * 0.03; // malé neutrální ambientní světlo
    float ao = texture(material.aoMap, TexCoords).r;
    ambient *= ao;

    // Kombinace světla
    vec3 color = ambient + (diffuse + specular) * light.diffuse * NdotL;
    color *= attenuation;

    return color;
}

// calculates the color when using a spot light.
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
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

    float offset = 0.03 * sin(uTime * 2.3 + fragPos.x * 10.0) + 0.02 * sin(uTime * 1.7 + fragPos.y * 12.0);

    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    // combine results
    vec3 ambient = light.ambient * vec3(texture(material.ambient, TexCoords));
    vec3 diffuse = light.diffuse * diff;
    vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));
    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;

    return (ambient + diffuse + specular);
}

//vec3 CalcSpotLightPBR(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
//{
//    vec3 N = normalize(normal);
//    vec3 V = normalize(viewDir);
//    vec3 L = normalize(light.position - fragPos);
//    vec3 H = normalize(V + L);
//
//    float NdotL = max(dot(N, L), 0.0);
//
//    // PBR Fresnel, NDF, Geometry
//    float NDF = DistributionGGX(N, H, roughness);
//    float G   = GeometrySmith(N, V, L, roughness);
//    vec3 F    = fresnelSchlick(max(dot(H, V), 0.0), F0);
//
//    vec3 specular = (NDF * G * F) / max(4.0 * max(dot(N, V),0.0) * NdotL, 0.001);
//    vec3 kS = F;
//    vec3 kD = vec3(1.0) - kS;
//    kD *= 1.0 - metalness;
//
//    vec3 diffuse = kD * albedo / 3.141592;
//
//    // attenuation
//    float distance = length(light.position - fragPos);
//    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * distance * distance);
//
//    // spotlight intensity
//    float theta = dot(L, normalize(-light.direction));
//    float epsilon = light.cutOff - light.outerCutOff;
//    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
//
//    // ambient
//    vec3 ambient = light.ambient * albedo;
//
//    return (ambient + (diffuse + specular) * light.diffuse * NdotL) * attenuation * intensity;
//}

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = 3.141592 * denom * denom;

    return nom / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

vec3 CalcIBLSpecular(vec3 R, float roughness, vec3 F0) {
    vec3 prefilteredColor;
    if (iblEnabled) {
        prefilteredColor = textureLod(environmentMap, R, roughness * 4.0).rgb;
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