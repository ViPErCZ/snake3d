#include <catch2/catch_all.hpp>

#include "../Renderer/Opengl/Material/Feature/HoleMapFeature.h"
#include "../Renderer/Opengl/Material/MaterialBuilder.h"
#include "../Renderer/Opengl/Material/TextureSlots.h"
#include "../Manager/ShaderFeature.h"

using Feature::HoleMapFeature;
using Feature::IMaterialFeature;
using Manager::ShaderFeature;
using Manager::ShaderFeatureMask;
using Material::MaterialBuilder;

// HoleMapFeature s nullptr texture - lze konstruovat bez GL kontextu.
// bind() by selhala na holeMap->bind() ale my testujeme jen flag(), clone()
// a kompozici v builderu.

TEST_CASE("HoleMapFeature reports FEATURE_HOLE_MAP flag") {
    HoleMapFeature feat(nullptr);
    CHECK(feat.flag() == static_cast<ShaderFeatureMask>(ShaderFeature::HoleMap));
}

TEST_CASE("HoleMapFeature::clone shares the texture handle") {
    const auto holeMap = std::make_shared<Manager::TextureManager>();
    HoleMapFeature feat(holeMap);

    const auto cloned = feat.clone();
    REQUIRE(cloned != nullptr);
    CHECK(cloned.get() != &feat);

    const auto castedClone = std::dynamic_pointer_cast<HoleMapFeature>(cloned);
    REQUIRE(castedClone != nullptr);
    // Clone should reuse the same TextureManager (immutable resource).
    CHECK(castedClone->getTexture().get() == holeMap.get());
}

TEST_CASE("MaterialBuilder accumulates features in order") {
    MaterialBuilder b;
    const auto f1 = std::make_shared<HoleMapFeature>(nullptr);
    const auto f2 = std::make_shared<HoleMapFeature>(nullptr);

    b.useMaster("basicShader").with(f1).with(f2);

    CHECK(b.masterName() == "basicShader");
    REQUIRE(b.featuresView().size() == 2);
    CHECK(b.featuresView()[0].get() == f1.get());
    CHECK(b.featuresView()[1].get() == f2.get());
}

TEST_CASE("MaterialBuilder featureMask is OR of all feature flags") {
    MaterialBuilder b;

    // Empty builder yields 0.
    CHECK(b.featureMask() == 0);

    // One HoleMap feature -> mask has only the HoleMap bit.
    b.with(std::make_shared<HoleMapFeature>(nullptr));
    CHECK(b.featureMask() == static_cast<ShaderFeatureMask>(ShaderFeature::HoleMap));

    // Two HoleMap features (unusual but valid) - mask stays the same since
    // OR is idempotent for the same bit.
    b.with(std::make_shared<HoleMapFeature>(nullptr));
    CHECK(b.featureMask() == static_cast<ShaderFeatureMask>(ShaderFeature::HoleMap));
}

TEST_CASE("MaterialBuilder::with ignores nullptr feature") {
    MaterialBuilder b;
    b.with(nullptr);
    CHECK(b.featuresView().empty());
    CHECK(b.featureMask() == 0);
}

TEST_CASE("TextureSlots values are unique") {
    // Compile-time sanity check that we haven't accidentally collided slots.
    using namespace Material::TextureSlots;
    const int all[] = {Albedo, Normal, Specular, ShadowArray, Metalness,
                       Roughness, EnvironmentMap, AoMap, HoleMap, PlanarReflection};
    constexpr int count = sizeof(all) / sizeof(all[0]);
    for (int i = 0; i < count; ++i) {
        for (int j = i + 1; j < count; ++j) {
            INFO("slot[" << i << "]=" << all[i] << " slot[" << j << "]=" << all[j]);
            CHECK(all[i] != all[j]);
        }
    }
}
