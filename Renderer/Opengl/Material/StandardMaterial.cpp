#include "StandardMaterial.h"

Material::StandardMaterial::StandardMaterial(shared_ptr<ShaderManager> baseShader,
                                             shared_ptr<ShaderManager> shadowDepthShader,
                                             const shared_ptr<WorldEnvironment> &worldEnv)
    : shader(std::move(baseShader)), shadowDepthShader(std::move(shadowDepthShader)), worldEnvironment(worldEnv) {
};

Material::StandardMaterial::~StandardMaterial() = default;

std::shared_ptr<TextureManager> Material::StandardMaterial::getAlbedo() const {
    return albedo;
}

void Material::StandardMaterial::setAlbedo(const std::shared_ptr<TextureManager> &albedo) {
    this->albedo = albedo;
}

std::shared_ptr<TextureManager> Material::StandardMaterial::getNormal() const {
    return normal;
}

shared_ptr<TextureManager> Material::StandardMaterial::getShadow() const {
    return shadow;
}

void Material::StandardMaterial::setNormalEnabled(const bool normal_enabled) {
    this->normal_enabled = normal_enabled;
}

void Material::StandardMaterial::bind(const glm::vec3 &posView, const glm::mat4 &view, const glm::mat4 &projection,
                                      const glm::mat4 &model) const {
    shader->use();
    shader->setMat4("view", view);
    shader->setMat4("projection", projection);
    shader->setMat4("model", model);
    shader->setVec3("viewPos", posView);
    shader->setBool("useMaterial", false);
    shader->setBool("useBones", false);
    shader->setBool("shadowsEnable", false);
    shader->setBool("iblEnabled", false);
    shader->setFloat("ambientLightColorIntensity", 0.05);
    shader->setVec3("ambientLightColor", color);
    shader->setBool("fogEnable", false);
    shader->setVec2("uvScale", UVScale);
    shader->setVec2("uvOffset", UVOffset);
    shader->setInt("material.ambient", 0);
    shader->setInt("material.diffuse", 1);
    shader->setInt("material.specular", 2);
    shader->setInt("shadowMap", 3);
    shader->setInt("metalness", 4);
    shader->setInt("roughness", 5);
    shader->setInt("environmentMap", 6);
    shader->setInt("aoMap", 7);

    if (shadowEnabled) {
        shader->setBool("shadowsEnable", true);
        if (shadow.get() && shadow.get()->hasTexture()) {
            shadow.get()->bind(3);
        }
    }

    if (worldEnvironment) {
        if (worldEnvironment->getEnvironment()) {
            shader->setVec3("ambientLightColor", worldEnvironment->getEnvironment()->getAmbientLight().color);
            shader->setFloat("ambientLightColorIntensity",
                             worldEnvironment->getEnvironment()->getAmbientLight().intensity);
        } else {
            shader->setVec3("ambientLightColor", color);
        }
    }

    shader->setFloat("material.shininess", shininess);

    // directional light
    if (directionalLight) {
        directionalLight->bind(shader.get());
        shader->setBool("directionLightEnable", true);
    } else {
        shader->setBool("directionLightEnable", false);
        shader->setVec3("lightPos", {0, 0,0});
    }

    // POINT LIGHT
    // --------------------------------
    shader->setInt("numPointLights", static_cast<int>(pointLights.size()));
    int index = 0;
    for (const auto & pointLight : pointLights) {
        pointLight->bind(shader.get(), index);
        index++;
    }
    // --------------------------------
    // END POINT LIGHT

    // SPOT LIGHT
    // --------------------------------
    shader->setInt("numSpotLights", static_cast<int>(spotLights.size()));
    index = 0;
    for (const auto & spotLight : spotLights) {
        spotLight->bind(shader.get(), index);
        index++;
    }
    // -----------------------------------------------
    // END SPOT LIGHT

    if (albedo && albedo.get()->hasTexture()) {
        shader->setBool("useMaterial", false);
        albedo.get()->bind(0);
    } else {
        shader->setBool("useMaterial", true);
    }
    const bool shaderNormal = normal_enabled && normal && normal.get()->hasTexture();
    shader->setBool("normalMapEnabled", shaderNormal);
    if (shaderNormal) {
        normal.get()->bind(1);
    }

    if (specular && specular.get()->hasTexture()) {
        specular.get()->bind(2);
        shader->setBool("specularMapEnabled", true);
    } else {
        shader->setBool("specularMapEnabled", false);
    }

    if (metalness && metalness.get()->hasTexture()) {
        shader->setInt("metalness", 4);
        shader->setBool("pbrEnabled", true);
        metalness.get()->bind(4);
    }

    if (roughness && roughness.get()->hasTexture()) {
        shader->setInt("roughness", 5);
        shader->setBool("pbrEnabled", true);
        roughness.get()->bind(5);
    }

    if (aoMap && aoMap.get()->hasTexture()) {
        shader->setInt("aoMap", 7);
        aoMap.get()->bind(7);
    }

    if (environmentMap && environmentMap.get()->hasTexture()) {
        shader->setBool("iblEnabled", true);
        environmentMap.get()->cubeBind(6);
    }
}

void Material::StandardMaterial::bindShadow(const glm::mat4 &model) const {
    shadowDepthShader->use();
    shadowDepthShader->setMat4("model", model);
}

void Material::StandardMaterial::unbind() const {
    if (albedo) {
        albedo.get()->unbind(0);
    }
    if (normal_enabled) {
        normal.get()->unbind(1);
    }
    if (specular) {
        specular.get()->unbind(2);
    }
    if (shadow) {
        shadow.get()->unbind(3);
    }
    if (metalness) {
        metalness.get()->unbind(4);
    }
    if (roughness) {
        roughness.get()->unbind(5);
    }
    if (environmentMap) {
        environmentMap.get()->unbind(6);
    }
    if (aoMap) {
        aoMap.get()->unbind(7);
    }
}

glm::vec3 Material::StandardMaterial::getColor() const {
    return color;
}

void Material::StandardMaterial::setColor(const glm::vec3 &color) {
    this->color = color;
}

bool Material::StandardMaterial::isShadowEnabled() const {
    return shadowEnabled;
}

void Material::StandardMaterial::setShadow(const bool shadow_enabled) {
    shadowEnabled = shadow_enabled;
}

void Material::StandardMaterial::setDirectionalLight(const shared_ptr<DirectionalLight> &directional_light) {
    directionalLight = directional_light;
}

void Material::StandardMaterial::addSpotLight(const shared_ptr<SpotLight> &spot_light) {
    spotLights.push_back(spot_light);
}

void Material::StandardMaterial::addPointLight(const shared_ptr<PointLight> &point_light) {
    pointLights.push_back(point_light);
}

void Material::StandardMaterial::setShininess(const float shininess) {
    this->shininess = shininess;
}

shared_ptr<TextureManager> Material::StandardMaterial::getRoughness() const {
    return roughness;
}

void Material::StandardMaterial::setRoughness(const shared_ptr<TextureManager> &roughness) {
    this->roughness = roughness;
}

shared_ptr<TextureManager> Material::StandardMaterial::getMetalness() const {
    return metalness;
}

void Material::StandardMaterial::setMetalness(const shared_ptr<TextureManager> &metalness) {
    this->metalness = metalness;
}

void Material::StandardMaterial::setEnvironmentMap(const shared_ptr<TextureManager> &environment_map) {
    environmentMap = environment_map;
}

void Material::StandardMaterial::setAoMap(const shared_ptr<TextureManager> &ao_map) {
    aoMap = ao_map;
}

void Material::StandardMaterial::setNormal(const std::shared_ptr<TextureManager> &normal) {
    this->normal = normal;
}

std::shared_ptr<TextureManager> Material::StandardMaterial::getSpecular() const {
    return specular;
}

void Material::StandardMaterial::setSpecular(const std::shared_ptr<TextureManager> &specular) {
    this->specular = specular;
}

void Material::StandardMaterial::setShadow(const std::shared_ptr<TextureManager> &shadow) {
    this->shadow = shadow;
}
