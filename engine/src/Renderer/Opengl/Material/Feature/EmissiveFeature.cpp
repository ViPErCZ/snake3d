#include <snake3d/Renderer/Opengl/Material/Feature/EmissiveFeature.h>

#include <snake3d/Manager/MaterialUbo.h>
#include <snake3d/Manager/ShaderFeature.h>

namespace Feature {
    EmissiveFeature::EmissiveFeature(const glm::vec3& color, const float intensity)
        : color(color), intensity(intensity) {
    }

    Manager::ShaderFeatureMask EmissiveFeature::flag() const {
        return static_cast<Manager::ShaderFeatureMask>(Manager::ShaderFeature::EmissiveBloom);
    }

    void EmissiveFeature::bind(Manager::ShaderProgram& /*shader*/,
                               const Material::RenderContext& ctx) const {
        if (!ctx.materialData) return;
        auto& md = *ctx.materialData;
        md.material_emissive_color = color;
        md.material_emissive_intensity = intensity;
        md.material_emissiveEnabled = 1;
        if (ctx.materialDirty) *ctx.materialDirty = true;
    }

    void EmissiveFeature::unbind(Manager::ShaderProgram& /*shader*/) const {
    }

    std::shared_ptr<IMaterialFeature> EmissiveFeature::clone() const {
        return std::make_shared<EmissiveFeature>(color, intensity);
    }
} // Feature
