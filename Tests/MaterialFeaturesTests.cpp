#include <catch2/catch_all.hpp>

#include "../Renderer/Opengl/Material/Feature/AlbedoFeature.h"
#include "../Renderer/Opengl/Material/Feature/BonesFeature.h"
#include "../Renderer/Opengl/Material/Feature/FogFeature.h"
#include "../Renderer/Opengl/Material/Feature/IblFeature.h"
#include "../Renderer/Opengl/Material/Feature/LightingFeature.h"
#include "../Renderer/Opengl/Material/Feature/NormalMapFeature.h"
#include "../Renderer/Opengl/Material/Feature/PbrFeature.h"
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

TEST_CASE("LightingFeature flag always advertises DirectionalLight") {
    // Even with no directional light, the flag is set so the shader compiles
    // the FEATURE_DIRECTIONAL_LIGHT block. A late setDirectional() call then
    // just toggles the runtime directionLightEnable uniform without needing
    // a recompile.
    Feature::LightingFeature noDir(nullptr, {}, {});
    CHECK(noDir.flag() == static_cast<ShaderFeatureMask>(ShaderFeature::DirectionalLight));

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

// B6a additions: PBR / IBL / Bones features.

TEST_CASE("PbrFeature reports FEATURE_PBR") {
    Feature::PbrFeature f(nullptr, nullptr);
    CHECK(f.flag() == static_cast<ShaderFeatureMask>(ShaderFeature::PBR));
}

TEST_CASE("PbrFeature::clone shares textures") {
    const auto metal = std::make_shared<Manager::TextureManager>();
    const auto rough = std::make_shared<Manager::TextureManager>();
    const auto ao    = std::make_shared<Manager::TextureManager>();
    Feature::PbrFeature original(metal, rough, ao);
    const auto cloned = std::dynamic_pointer_cast<Feature::PbrFeature>(original.clone());
    REQUIRE(cloned != nullptr);
    CHECK(cloned->getMetalness().get() == metal.get());
    CHECK(cloned->getRoughness().get() == rough.get());
    CHECK(cloned->getAoMap().get() == ao.get());
}

TEST_CASE("IblFeature reports FEATURE_IBL") {
    Feature::IblFeature f(nullptr);
    CHECK(f.flag() == static_cast<ShaderFeatureMask>(ShaderFeature::IBL));
}

TEST_CASE("IblFeature::clone shares environment cubemap") {
    const auto env = std::make_shared<Manager::TextureManager>();
    Feature::IblFeature original(env);
    const auto cloned = std::dynamic_pointer_cast<Feature::IblFeature>(original.clone());
    REQUIRE(cloned != nullptr);
    CHECK(cloned->getEnvironmentMap().get() == env.get());
}

TEST_CASE("BonesFeature reports FEATURE_BONES") {
    Feature::BonesFeature f;
    CHECK(f.flag() == static_cast<ShaderFeatureMask>(ShaderFeature::Bones));
}

TEST_CASE("BonesFeature::clone copies useBones state") {
    Feature::BonesFeature original(true);
    const auto cloned = std::dynamic_pointer_cast<Feature::BonesFeature>(original.clone());
    REQUIRE(cloned != nullptr);
    CHECK(cloned->getUseBones());

    original.setUseBones(false);
    CHECK(cloned->getUseBones());  // independent
}
