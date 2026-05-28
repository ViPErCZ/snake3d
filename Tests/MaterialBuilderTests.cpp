#include <catch2/catch_all.hpp>

#include "../Renderer/Opengl/Material/Feature/FogFeature.h"
#include "../Renderer/Opengl/Material/Feature/HoleMapFeature.h"
#include "../Renderer/Opengl/Material/MaterialBuilder.h"
#include "../Renderer/Opengl/Material/TextureSlots.h"
#include <snake3d/Manager/ShaderFeature.h>

using Feature::HoleMapFeature;
using Feature::IMaterialFeature;
using Manager::ShaderFeature;
using Manager::ShaderFeatureMask;
using Material::MaterialBuilder;

// HoleMapFeature s nullptr texture - lze konstruovat bez GL kontextu.
// bind() by selhala na holeMap->bind() ale my testujeme jen flag(), clone()
// a kompozici v builderu.

TEST_CASE("HoleMapFeature reports zero flag (C2b - migrated to snippet)") {
    HoleMapFeature feat(nullptr);
    CHECK(feat.flag() == 0);
}

TEST_CASE("HoleMapFeature snippetPaths targets FragmentPre slot") {
    HoleMapFeature feat(nullptr);
    const auto paths = feat.snippetPaths();
    REQUIRE(paths.size() == 1);
    CHECK(paths.contains(Manager::MaterialSlot::FragmentPre));
    CHECK(paths.at(Manager::MaterialSlot::FragmentPre).find("hole_map_discard.glsl") != std::string::npos);
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

    // HoleMapFeature post-C2b vrací flag()=0 (kód je v snippetu, ne v
    // master shader compile-time flagu). featureMask zůstane 0.
    b.with(std::make_shared<HoleMapFeature>(nullptr));
    CHECK(b.featureMask() == 0);
}

TEST_CASE("MaterialBuilder::with ignores nullptr feature") {
    MaterialBuilder b;
    b.with(nullptr);
    CHECK(b.featuresView().empty());
    CHECK(b.featureMask() == 0);
}

namespace {
    // GL-free test feature: declares a snippet path so we can exercise
    // MaterialBuilder's snippet collection without touching real shaders.
    class TestSnippetFeature final : public Feature::IMaterialFeature {
    public:
        explicit TestSnippetFeature(Manager::MaterialSlot slot, std::string path)
            : slot_(slot), path_(std::move(path)) {}

        [[nodiscard]] ShaderFeatureMask flag() const override { return 0; }
        void bind(Manager::ShaderProgram&, const Material::RenderContext&) const override {}
        [[nodiscard]] std::shared_ptr<Feature::IMaterialFeature> clone() const override {
            return std::make_shared<TestSnippetFeature>(slot_, path_);
        }
        [[nodiscard]] std::map<Manager::MaterialSlot, std::string> snippetPaths() const override {
            return {{slot_, path_}};
        }

    private:
        Manager::MaterialSlot slot_;
        std::string path_;
    };
}

TEST_CASE("IMaterialFeature default snippetPaths returns empty") {
    // FogFeature nemá snippetPaths() override - dědí default empty z IMaterialFeature.
    // (HoleMapFeature po C2b vlastní override má a vrací FRAGMENT_PRE snippet.)
    Feature::FogFeature feat;
    CHECK(feat.snippetPaths().empty());
}

TEST_CASE("MaterialBuilder collects snippets from features") {
    MaterialBuilder b;
    b.useMaster("main3D");
    b.with(std::make_shared<TestSnippetFeature>(Manager::MaterialSlot::FragmentPre, "snippets/a.glsl"));
    b.with(std::make_shared<TestSnippetFeature>(Manager::MaterialSlot::FragmentPost, "snippets/b.glsl"));

    // Builder itself doesn't expose snippets directly; we verify through
    // featureMask + the snippetPaths() of the stored features.
    REQUIRE(b.featuresView().size() == 2);
    CHECK(b.featuresView()[0]->snippetPaths().at(Manager::MaterialSlot::FragmentPre) == "snippets/a.glsl");
    CHECK(b.featuresView()[1]->snippetPaths().at(Manager::MaterialSlot::FragmentPost) == "snippets/b.glsl");
}

TEST_CASE("ShaderHandle equality includes snippets") {
    Manager::ShaderHandle a{"main3D", 0, {{"@X", {"p1.glsl"}}}};
    Manager::ShaderHandle b{"main3D", 0, {{"@X", {"p1.glsl"}}}};
    Manager::ShaderHandle c{"main3D", 0, {{"@X", {"p2.glsl"}}}};
    Manager::ShaderHandle d{"main3D", 0, {}};

    CHECK(a == b);
    CHECK_FALSE(a == c);
    CHECK_FALSE(a == d);
}

TEST_CASE("ShaderHandle distinguishes snippet order") {
    // C2a: two paths in different order = different cache entries.
    Manager::ShaderHandle ab{"main3D", 0, {{"@X", {"a.glsl", "b.glsl"}}}};
    Manager::ShaderHandle ba{"main3D", 0, {{"@X", {"b.glsl", "a.glsl"}}}};
    CHECK_FALSE(ab == ba);
}

TEST_CASE("slotMarker maps every MaterialSlot to a non-empty string") {
    // C3: kompilátor garantuje, že každý slot v enumu má return v switch.
    // Test zachytí scenář, kdy někdo přidá novou enum entry a zapomene update
    // slotMarker (default `return ""` by tichý byt, ale tady to vyklouzne).
    for (const auto slot : Manager::kAllSlots) {
        const auto marker = Manager::slotMarker(slot);
        INFO("slot=" << static_cast<int>(slot));
        CHECK(!marker.empty());
        CHECK(marker.starts_with("@MATERIAL_"));
    }
}

TEST_CASE("isKnownMaterialPlaceholder accepts every slot marker") {
    for (const auto slot : Manager::kAllSlots) {
        const auto marker = std::string(Manager::slotMarker(slot));
        INFO("marker=" << marker);
        CHECK(Manager::isKnownMaterialPlaceholder(marker));
    }
}

TEST_CASE("isKnownMaterialPlaceholder rejects unknown marker") {
    CHECK_FALSE(Manager::isKnownMaterialPlaceholder("@MATERIAL_TYPO"));
    CHECK_FALSE(Manager::isKnownMaterialPlaceholder("@WHATEVER"));
    CHECK_FALSE(Manager::isKnownMaterialPlaceholder(""));
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
