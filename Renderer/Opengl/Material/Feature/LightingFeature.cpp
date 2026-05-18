#include "LightingFeature.h"

namespace Feature {
    LightingFeature::LightingFeature(std::shared_ptr<Lights::DirectionalLight> directional,
                                     std::vector<std::shared_ptr<Lights::PointLight>> points,
                                     std::vector<std::shared_ptr<Lights::SpotLight>> spots)
        : directional(std::move(directional)),
          points(std::move(points)),
          spots(std::move(spots)) {
    }

    Manager::ShaderFeatureMask LightingFeature::flag() const {
        Manager::ShaderFeatureMask mask = 0;
        if (directional) {
            mask |= static_cast<Manager::ShaderFeatureMask>(Manager::ShaderFeature::DirectionalLight);
        }
        // PointLights / SpotLights aren't gated by #ifdef in basic.fs (their
        // loops run unconditionally driven by numPointLights/numSpotLights);
        // adding their flags would compile identical programs but split the
        // permutation cache pointlessly.
        return mask;
    }

    void LightingFeature::bind(Manager::ShaderManager& shader,
                               const Material::RenderContext& /*ctx*/) const {
        if (directional) {
            directional->bind(&shader);
            shader.setBool("directionLightEnable", true);
        } else {
            shader.setBool("directionLightEnable", false);
            shader.setVec3("lightPos", glm::vec3(0.0f));
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
