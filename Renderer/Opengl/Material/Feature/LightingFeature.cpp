#include "LightingFeature.h"

#include <glm/gtc/constants.hpp>

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

        // D1.1d: point + spot lights migrated to MaterialData UBO. We mirror
        // the legacy SpotLight::bind transformations here (normalize(dir-pos),
        // cos(radians(cutOff)), bool->int) so shader-side code reads the same
        // values that pre-D1.1d setUniform calls produced.
        if (ctx.materialData) {
            int pIdx = 0;
            for (const auto& pl : points) {
                if (pl && pl->isVisible() && pIdx < 8) {
                    auto& dst = ctx.materialData->material_pointLights[pIdx];
                    dst.position  = pl->getPosition();
                    dst.ambient   = pl->getAmbient();
                    dst.diffuse   = pl->getDiffuse();
                    dst.specular  = pl->getSpecular();
                    dst.constant  = pl->getConstant();
                    dst.linear    = pl->getLinear();
                    dst.quadratic = pl->getQuadratic();
                    ++pIdx;
                }
            }
            ctx.materialData->material_numPointLights = pIdx;

            int sIdx = 0;
            for (const auto& sl : spots) {
                if (sl && sl->isVisible() && sIdx < 8) {
                    auto& dst = ctx.materialData->material_spotLights[sIdx];
                    dst.position    = sl->getPosition();
                    dst.direction   = glm::normalize(sl->getDirection() - sl->getPosition());
                    dst.ambient     = sl->getAmbient();
                    dst.diffuse     = sl->getDiffuse();
                    dst.specular    = sl->getSpecular();
                    dst.constant    = sl->getConstant();
                    dst.linear      = sl->getLinear();
                    dst.quadratic   = sl->getQuadratic();
                    dst.cutOff      = glm::cos(glm::radians(sl->getCutOff()));
                    dst.outerCutOff = glm::cos(glm::radians(sl->getOuterCutOff()));
                    dst.pulse       = sl->isPulse() ? 1 : 0;
                    ++sIdx;
                }
            }
            ctx.materialData->material_numSpotLights = sIdx;
            if (ctx.materialDirty) *ctx.materialDirty = true;
        }
    }

    std::shared_ptr<IMaterialFeature> LightingFeature::clone() const {
        // Light objekty jsou sdílené - světla jsou managed scene-wise a
        // klonování materiálu nesmí oddělit instanci od scene lighting state.
        return std::make_shared<LightingFeature>(directional, points, spots);
    }
} // Feature
