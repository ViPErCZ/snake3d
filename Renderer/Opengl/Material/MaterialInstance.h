#ifndef SNAKE3_MATERIALINSTANCE_H
#define SNAKE3_MATERIALINSTANCE_H

#include <memory>
#include <vector>

#include "BaseMaterial.h"
#include "Feature/IMaterialFeature.h"
#include "RenderContext.h"
#include <snake3d/Manager/MaterialUbo.h>
#include <snake3d/Manager/ShaderProgram.h>

namespace Material {
    // Materiál sestavený z features přes MaterialBuilder. Drží konkrétní
    // ShaderProgram program (returnem z ShaderRegistry pro daný feature
    // mask) a list features. bind() iteruje features v pořadí přidání;
    // features zapisují své uniformy a binduje textury.
    //
    // V B4 fázi je MaterialInstance samostatná hierarchie vedle dnešního
    // StandardMaterial - mesh renderer (StandardMesh::render) ještě
    // neumí přes ni renderovat. To přijde v B5 spolu s migrací plane na
    // builder.
    class MaterialInstance final : public BaseMaterial {
    public:
        MaterialInstance(std::shared_ptr<Manager::ShaderProgram> program,
                         std::vector<std::shared_ptr<Feature::IMaterialFeature>> features);

        void bind(const RenderContext& ctx) const;
        void unbind() const;

        // Shadow-map pre-pass entry: hledá ShadowFeature ve features a
        // deleguje na něj. Vrací true pokud feature shadow shader nabindla
        // (a tedy je smysluplné nasledně volat glDrawElements). Bez ShadowFeature
        // materiál do shadow mapy nepřispívá - materiál není shadow caster.
        bool bindShadow(const glm::mat4& model) const;

        // Vrátí shadowDepthShader z ShadowFeature, nebo nullptr pokud tato
        // feature není v sestavě. AnimationArrayMesh::renderMesh ho potřebuje
        // pro per-mesh model uniform v shadow pass.
        [[nodiscard]] std::shared_ptr<Manager::ShaderProgram> getShadowProgram() const;

        [[nodiscard]] std::shared_ptr<BaseMaterial> clone() const override;

        [[nodiscard]] std::shared_ptr<Manager::ShaderProgram> getProgram() const { return program; }
        [[nodiscard]] const std::vector<std::shared_ptr<Feature::IMaterialFeature>>& getFeatures() const { return features; }

    private:
        std::shared_ptr<Manager::ShaderProgram> program;
        std::vector<std::shared_ptr<Feature::IMaterialFeature>> features;

        // Per-material UBO (binding slot 1). D1.2b uploaded data dormant;
        // D1.2c flips features (starting with AlbedoFeature) to write into
        // `cpu` via the RenderContext pointer and read on the GPU as
        // `material_*` instead of legacy setUniform names. `cpu` is mutable
        // because MaterialInstance::bind() is const but features mutate the
        // shadow through ctx.materialData during that call.
        mutable Manager::MaterialUbo materialUbo;
        mutable Manager::MaterialDataStd140 cpu{};
        mutable bool dirty = true;
    };
} // Material

#endif //SNAKE3_MATERIALINSTANCE_H
