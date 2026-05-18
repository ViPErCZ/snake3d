#include <catch2/catch_all.hpp>

#include "../Renderer/Opengl/Material/Feature/AlbedoFeature.h"
#include "../Renderer/Opengl/Material/Feature/FogFeature.h"
#include "../Renderer/Opengl/Material/Feature/LightingFeature.h"
#include "../Renderer/Opengl/Material/Feature/NormalMapFeature.h"
#include "../Renderer/Opengl/Material/Feature/PlanarReflectionFeature.h"
#include "../Renderer/Opengl/Material/Feature/ShadowFeature.h"
#include "../Renderer/Opengl/Material/Feature/SpecularFeature.h"
#include "../Renderer/Opengl/Material/Feature/UvTransformFeature.h"
#include "../Manager/ShaderFeature.h"

using Manager::ShaderFeature;
using Manager::ShaderFeatureMask;

// flag() tests - deterministic from constructor args, no GL needed.

TEST_CASE("FogFeature reports FEATURE_FOG") {
    CHECK(Feature::FogFeature().flag() == static_cast<ShaderFeatureMask>(ShaderFeature::Fog));
}

TEST_CASE("UvTransformFeature has no shader flag") {
    CHECK(Feature::UvTransformFeature().flag() == 0);
}

TEST_CASE("AlbedoFeature has no shader flag") {
    CHECK(Feature::AlbedoFeature().flag() == 0);
}

TEST_CASE("NormalMapFeature reports FEATURE_NORMAL_MAP") {
    Feature::NormalMapFeature f(nullptr);
    CHECK(f.flag() == static_cast<ShaderFeatureMask>(ShaderFeature::NormalMap));
}

TEST_CASE("SpecularFeature has no shader flag") {
    Feature::SpecularFeature f(nullptr);
    CHECK(f.flag() == 0);
}

TEST_CASE("ShadowFeature reports FEATURE_SHADOWS") {
    Feature::ShadowFeature f(nullptr);
    CHECK(f.flag() == static_cast<ShaderFeatureMask>(ShaderFeature::Shadows));
}

TEST_CASE("PlanarReflectionFeature has no shader flag") {
    Feature::PlanarReflectionFeature f(nullptr);
    CHECK(f.flag() == 0);
}

TEST_CASE("LightingFeature flag depends on directional light presence") {
    // No directional - no flag.
    Feature::LightingFeature noDir(nullptr, {}, {});
    CHECK(noDir.flag() == 0);

    // With directional - DirectionalLight bit set.
    auto dir = std::make_shared<Lights::DirectionalLight>();
    Feature::LightingFeature withDir(dir, {}, {});
    CHECK(withDir.flag() == static_cast<ShaderFeatureMask>(ShaderFeature::DirectionalLight));
}

// clone() tests - new instance, shared resources where applicable.

TEST_CASE("FogFeature::clone copies state") {
    Feature::FogFeature original(false);
    const auto cloned = std::dynamic_pointer_cast<Feature::FogFeature>(original.clone());
    REQUIRE(cloned != nullptr);
    CHECK(cloned->isEnabled() == false);

    original.setEnabled(true);
    CHECK(cloned->isEnabled() == false);  // independent
}

TEST_CASE("UvTransformFeature::clone copies scale and offset") {
    Feature::UvTransformFeature original(glm::vec2(48.0f), glm::vec2(0.5f, 0.25f));
    const auto cloned = std::dynamic_pointer_cast<Feature::UvTransformFeature>(original.clone());
    REQUIRE(cloned != nullptr);
    CHECK(cloned->getScale() == glm::vec2(48.0f));
    CHECK(cloned->getOffset() == glm::vec2(0.5f, 0.25f));

    original.setScale(glm::vec2(1.0f));
    CHECK(cloned->getScale() == glm::vec2(48.0f));  // independent
}

TEST_CASE("AlbedoFeature::clone copies color/alpha and shares texture") {
    const auto tex = std::make_shared<Manager::TextureManager>();
    Feature::AlbedoFeature original(tex);
    original.setColor(glm::vec3(0.1f, 0.2f, 0.3f));
    original.setAlpha(0.5f);

    const auto cloned = std::dynamic_pointer_cast<Feature::AlbedoFeature>(original.clone());
    REQUIRE(cloned != nullptr);
    CHECK(cloned->getAlbedo().get() == tex.get());  // shared
    REQUIRE(cloned->getColor().has_value());
    CHECK(cloned->getColor()->x == Catch::Approx(0.1f));
    CHECK(cloned->getAlpha() == Catch::Approx(0.5f));

    original.clearColor();
    CHECK(cloned->getColor().has_value());  // independent
}

TEST_CASE("NormalMapFeature::clone shares texture") {
    const auto tex = std::make_shared<Manager::TextureManager>();
    Feature::NormalMapFeature original(tex);
    const auto cloned = std::dynamic_pointer_cast<Feature::NormalMapFeature>(original.clone());
    REQUIRE(cloned != nullptr);
    CHECK(cloned->getTexture().get() == tex.get());
}

TEST_CASE("ShadowFeature::clone shares texture and depth shader") {
    const auto tex = std::make_shared<Manager::TextureManager>();
    Feature::ShadowFeature original(tex, nullptr);
    const auto cloned = std::dynamic_pointer_cast<Feature::ShadowFeature>(original.clone());
    REQUIRE(cloned != nullptr);
    CHECK(cloned->getShadowArray().get() == tex.get());
}

TEST_CASE("PlanarReflectionFeature::clone copies enabled flag and shares texture") {
    const auto tex = std::make_shared<Manager::TextureManager>();
    Feature::PlanarReflectionFeature original(tex, true);
    const auto cloned = std::dynamic_pointer_cast<Feature::PlanarReflectionFeature>(original.clone());
    REQUIRE(cloned != nullptr);
    CHECK(cloned->isEnabled());
    CHECK(cloned->getTexture().get() == tex.get());

    original.setEnabled(false);
    CHECK(cloned->isEnabled());  // independent
}

TEST_CASE("LightingFeature::clone shares directional light") {
    const auto dir = std::make_shared<Lights::DirectionalLight>();
    Feature::LightingFeature original(dir, {}, {});
    const auto cloned = std::dynamic_pointer_cast<Feature::LightingFeature>(original.clone());
    REQUIRE(cloned != nullptr);
    CHECK(cloned->getDirectional().get() == dir.get());
}
