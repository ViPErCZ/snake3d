#include "MaterialInstance.h"

#include "Feature/ShadowFeature.h"

namespace Material {
    MaterialInstance::MaterialInstance(std::shared_ptr<Manager::ShaderProgram> program,
                                       std::vector<std::shared_ptr<Feature::IMaterialFeature>> features)
        : program(std::move(program)), features(std::move(features)) {
        // Seed cpu shadow with the same defaults the legacy uniforms had at
        // their setUniform sites. D1.2b uploads but no shader reads the block
        // yet -- D1.2c flips features to read material_* and these defaults
        // become the actual rendering parameters. Keeping them identical now
        // guarantees pixel-stable behaviour through the migration.
        cpu.material_alpha = 1.0F;
        cpu.material_ambientLightColor = glm::vec3(1.0F);
        cpu.material_ambientLightColorIntensity = 1.0F;
        cpu.material_uvScale = glm::vec2(1.0F);
        cpu.material_uvOffset = glm::vec2(0.0F);
        cpu.material_rainSpeed = 0.2F;
        cpu.material_rainDensity = 20.0F;
        cpu.material_clipPlane = glm::vec4(0.0F, 0.0F, 1.0F, 1000.0F);
        // All int/flag members already zero from in-class {} initialisers.
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

        // D1.2c: expose cpu shadow + dirty flag to features through ctx so
        // migrated features (AlbedoFeature in this step) can write directly
        // into the per-material UBO instead of calling setUniform on the
        // legacy globals. Pointers are scoped to this bind() call.
        ctx.materialData = &cpu;
        ctx.materialDirty = &dirty;

        // Run features first so any UBO writes happen before upload below.
        // Features that haven't migrated yet still call shader.setX(...) --
        // both paths coexist throughout D1.2c..D1.2d.
        for (const auto& f : features) {
            if (f) f->bind(*program, ctx);
        }

        // Upload + bind per-material UBO at binding slot 1. Dirty is true on
        // first bind (ctor seeds defaults) and whenever a feature wrote to
        // *ctx.materialData this frame. Lazy-init on first upload allocates
        // the GL buffer on the render thread (safe even though
        // MaterialInstance may be constructed on a loader thread before GLEW
        // is ready).
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
