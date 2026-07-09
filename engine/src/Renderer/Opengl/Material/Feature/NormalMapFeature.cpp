#include <snake3d/Renderer/Opengl/Material/Feature/NormalMapFeature.h>

#include <snake3d/Renderer/Opengl/Material/TextureSlots.h>
#include <snake3d/Manager/MaterialUbo.h>

namespace Feature {
    NormalMapFeature::NormalMapFeature(std::shared_ptr<Manager::TextureManager> normal)
        : normal(std::move(normal)) {
    }

    void NormalMapFeature::bind(Manager::ShaderProgram& shader,
                                const Material::RenderContext& ctx) const {
        shader.setInt("material.diffuse", Material::TextureSlots::Normal);
        const bool active = normal && normal->hasTexture();
        // D1.2d.2: normalMapEnabled migrated to MaterialData UBO
        // (material_normalMapEnabled). Sampler bindings stay legacy
        // (samplers can't live in a UBO).
        if (ctx.materialData) {
            ctx.materialData->material_normalMapEnabled = active ? 1 : 0;
            if (ctx.materialDirty) *ctx.materialDirty = true;
        }
        if (active) {
            normal->bind(Material::TextureSlots::Normal);
        }
    }

    void NormalMapFeature::unbind(Manager::ShaderProgram& /*shader*/) const {
        if (normal) {
            normal->unbind(Material::TextureSlots::Normal);
        }
    }

    std::shared_ptr<IMaterialFeature> NormalMapFeature::clone() const {
        return std::make_shared<NormalMapFeature>(normal);
    }
} // Feature
