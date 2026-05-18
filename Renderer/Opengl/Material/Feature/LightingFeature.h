#ifndef SNAKE3_LIGHTINGFEATURE_H
#define SNAKE3_LIGHTINGFEATURE_H

#include <memory>
#include <vector>

#include "IMaterialFeature.h"
#include "../../../../Lights/DirectionalLight.h"
#include "../../../../Lights/PointLight.h"
#include "../../../../Lights/SpotLight.h"

namespace Feature {
    // Slunce + bodová + spotová světla v jedné feature. Bind iteruje
    // viditelné světla a nastaví numPointLights/numSpotLights count.
    //
    // Feature flag = DirectionalLight pokud má slunce nastavené; PointLights
    // a SpotLights neaktivují žádný #ifdef (basic.fs zatím nemá - jen běží
    // for-loop podle count). Pokud má smysl, B6+ může features rozdělit.
    class LightingFeature final : public IMaterialFeature {
    public:
        LightingFeature(std::shared_ptr<Lights::DirectionalLight> directional,
                        std::vector<std::shared_ptr<Lights::PointLight>> points,
                        std::vector<std::shared_ptr<Lights::SpotLight>> spots);

        [[nodiscard]] Manager::ShaderFeatureMask flag() const override;

        void bind(Manager::ShaderManager& shader,
                  const Material::RenderContext& ctx) const override;
        [[nodiscard]] std::shared_ptr<IMaterialFeature> clone() const override;

        void setDirectional(std::shared_ptr<Lights::DirectionalLight> d) { directional = std::move(d); }
        void setPoints(std::vector<std::shared_ptr<Lights::PointLight>> p) { points = std::move(p); }
        void setSpots(std::vector<std::shared_ptr<Lights::SpotLight>> s) { spots = std::move(s); }

        [[nodiscard]] std::shared_ptr<Lights::DirectionalLight> getDirectional() const { return directional; }

    private:
        std::shared_ptr<Lights::DirectionalLight> directional;
        std::vector<std::shared_ptr<Lights::PointLight>> points;
        std::vector<std::shared_ptr<Lights::SpotLight>> spots;
    };
} // Feature

#endif //SNAKE3_LIGHTINGFEATURE_H
