#include "MaterialInstance.h"

#include "Feature/ShadowFeature.h"

namespace Material {
    MaterialInstance::MaterialInstance(std::shared_ptr<Manager::ShaderProgram> program,
                                       std::vector<std::shared_ptr<Feature::IMaterialFeature>> features)
        : program(std::move(program)), features(std::move(features)) {
        cpu.material_alpha = 1.0F;
        cpu.material_ambientLightColor = glm::vec3(1.0F);
        cpu.material_ambientLightColorIntensity = 1.0F;
        cpu.material_uvScale = glm::vec2(1.0F);
        cpu.material_uvOffset = glm::vec2(0.0F);
        cpu.material_rainSpeed = 0.2F;
        cpu.material_rainDensity = 20.0F;
        cpu.material_clipPlane = glm::vec4(0.0F, 0.0F, 1.0F, 1000.0F);
    }

    void MaterialInstance::bind(const RenderContext& ctx) const {
        if (!program) {
            return;
        }
        program->use();
        program->setMat4("model", ctx.model);
        ctx.materialData = &cpu;
        ctx.materialDirty = &dirty;

        for (const auto& f : features) {
            if (f) f->bind(*program, ctx);
        }

        if (dirty) {
            materialUbo.upload(cpu);
            dirty = false;
        }
        materialUbo.bind();

        // Detach pointers so a stale ctx reused for another draw can't
        // accidentally mutate this instance's shadow.
        ctx.materialData = nullptr;
        ctx.materialDirty = nullptr;
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

    std::shared_ptr<Manager::ShaderProgram> MaterialInstance::getShadowProgram() const {
        for (const auto& f : features) {
            if (const auto shadow = std::dynamic_pointer_cast<Feature::ShadowFeature>(f)) {
                return shadow->getShadowDepthShader();
            }
        }
        return nullptr;
    }

    std::shared_ptr<BaseMaterial> MaterialInstance::clone() const {
        // Program (GL handle) je shared - klonujeme jen kompozici features.
        std::vector<std::shared_ptr<Feature::IMaterialFeature>> clonedFeatures;
        clonedFeatures.reserve(features.size());
        for (const auto& f : features) {
            clonedFeatures.push_back(f ? f->clone() : nullptr);
        }
        auto cloned = std::make_shared<MaterialInstance>(program, std::move(clonedFeatures));
        // Carry over UBO shadow so the clone renders with identical material
        // params from the start. dirty stays true on the new instance -- its
        // own GL UBO is lazy-allocated and uploaded on first bind.
        cloned->cpu = this->cpu;
        return cloned;
    }
} // Material
