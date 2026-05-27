#include "LightingFeature.h"

#include "../../../../Manager/MaterialUbo.h"

namespace Feature {
    LightingFeature::LightingFeature(std::shared_ptr<Lights::DirectionalLight> directional,
                                     std::vector<std::shared_ptr<Lights::PointLight>> points,
                                     std::vector<std::shared_ptr<Lights::SpotLight>> spots)
        : directional(std::move(directional)),
          points(std::move(points)),
          spots(std::move(spots)) {
    }

    Manager::ShaderFeatureMask LightingFeature::flag() const {
        // Always advertise DirectionalLight so the FEATURE_DIRECTIONAL_LIGHT
        // block compiles even when directional is set later than build() (e.g.
        // SnakeMeshNode3D ctor builds the tile material, then setDirectional
        // hooks up the scene light). At runtime `directionLightEnable` skips
        // the block when no light is bound. The cost is a few KB of compiled
        // shader code per permutation that never runs, which is negligible.
        //
        // PointLights / SpotLights aren't gated by #ifdef in basic.fs (their
        // loops run unconditionally driven by numPointLights/numSpotLights);
        // adding their flags would split the permutation cache pointlessly.
        return static_cast<Manager::ShaderFeatureMask>(Manager::ShaderFeature::DirectionalLight);
    }

    void LightingFeature::bind(Manager::ShaderProgram& shader,
                               const Material::RenderContext& ctx) const {
        // D1.1c-fix: directional light fields (direction, ambient, diffuse,
        // specular) live per-material in MaterialData UBO. Snake body/head
        // materials use their own (intentionally dim) DirectionalLight from
        // PlayerScene / RemoteSnakeScene -- if we shoved this into FrameData
        // they'd silently get the global brighter light. Per-material UBO
        // copy mirrors the pre-D1 per-program `dirLight.*` uniform model.
        // The opt-out gate (material_directionLightEnable) also lives here.
        // directional->bind() still runs (when present) so material.shininess
        // + sampler ints land on the program.
        if (ctx.materialData) {
            ctx.materialData->material_directionLightEnable = directional ? 1 : 0;
            if (directional) {
                ctx.materialData->material_dirLight_direction = directional->getDirection();
                ctx.materialData->material_dirLight_ambient   = directional->getAmbient();
                ctx.materialData->material_dirLight_diffuse   = directional->getDiffuse();
                ctx.materialData->material_dirLight_specular  = directional->getSpecular();
            }
            if (ctx.materialDirty) *ctx.materialDirty = true;
        }
        if (directional) {
            directional->bind(&shader);
        }

        int index = 0;
        for (const auto& pl : points) {
            if (pl && pl->isVisible()) {
                pl->bind(&shader, index);
                ++index;
            }
        }
        shader.setInt("numPointLights", index);

        index = 0;
        for (const auto& sl : spots) {
            if (sl && sl->isVisible()) {
                sl->bind(&shader, index);
                ++index;
            }
        }
        shader.setInt("numSpotLights", index);
    }

    std::shared_ptr<IMaterialFeature> LightingFeature::clone() const {
        // Light objekty jsou sdílené - světla jsou managed scene-wise a
        // klonování materiálu nesmí oddělit instanci od scene lighting state.
        return std::make_shared<LightingFeature>(directional, points, spots);
    }
} // Feature
