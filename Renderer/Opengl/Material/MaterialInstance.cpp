#include "MaterialInstance.h"

#include "Feature/ShadowFeature.h"

namespace Material {
    MaterialInstance::MaterialInstance(std::shared_ptr<Manager::ShaderManager> program,
                                       std::vector<std::shared_ptr<Feature::IMaterialFeature>> features)
        : program(std::move(program)), features(std::move(features)) {
    }

    void MaterialInstance::bind(const RenderContext& ctx) const {
        if (!program) {
            return;
        }
        program->use();

        // Core uniformy - mat/view/proj/viewPos/uTime. Stejné názvy které
        // dnes nastavuje StandardMaterial::bind. setBool/setInt/setMat4 na
        // neexistující uniform jsou v GL bezpečné no-ops (location -1).
        program->setMat4("model", ctx.model);
        program->setMat4("view", ctx.view);
        program->setMat4("projection", ctx.projection);
        program->setVec3("viewPos", ctx.viewPos);
        program->setFloat("uTime", ctx.uTime);

        for (const auto& f : features) {
            if (f) f->bind(*program, ctx);
        }
    }

    void MaterialInstance::unbind() const {
        if (!program) {
            return;
        }
        for (const auto& f : features) {
            if (f) f->unbind(*program);
        }
    }

    bool MaterialInstance::bindShadow(const glm::mat4& model) const {
        for (const auto& f : features) {
            if (const auto shadow = std::dynamic_pointer_cast<Feature::ShadowFeature>(f)) {
                return shadow->bindShadow(model);
            }
        }
        return false;
    }

    std::shared_ptr<BaseMaterial> MaterialInstance::clone() const {
        // Program (GL handle) je shared - klonujeme jen kompozici features.
        std::vector<std::shared_ptr<Feature::IMaterialFeature>> clonedFeatures;
        clonedFeatures.reserve(features.size());
        for (const auto& f : features) {
            clonedFeatures.push_back(f ? f->clone() : nullptr);
        }
        return std::make_shared<MaterialInstance>(program, std::move(clonedFeatures));
    }
} // Material
