#include <catch2/catch_all.hpp>

#include <memory>

#include <nlohmann/json.hpp>

#include <snake3d/Manager/ResourceManager.h>
#include <snake3d/Manager/TextureManager.h>
#include <snake3d/Renderer/Opengl/Material/Feature/AlbedoFeature.h>
#include <snake3d/Renderer/Opengl/Material/Feature/BonesFeature.h>
#include <snake3d/Renderer/Opengl/Material/Feature/IblFeature.h>
#include <snake3d/Renderer/Opengl/Material/Feature/IMaterialFeature.h>
#include <snake3d/Renderer/Opengl/Material/Feature/NormalMapFeature.h>
#include <snake3d/Renderer/Opengl/Material/Feature/PbrFeature.h>
#include <snake3d/Renderer/Opengl/Material/Feature/SpecularFeature.h>
#include <snake3d/Renderer/Opengl/Material/Feature/UvTransformFeature.h>
#include <snake3d/Resource/FeatureRegistry.h>
#include <snake3d/Resource/MaterialLoader.h>

using Manager::ResourceManager;
using Manager::TextureManager;
using Resource::FeatureRegistry;
using Resource::MaterialSpec;
using Resource::loadFromJson;
using Resource::registerBuiltinFeatures;

namespace {
    std::shared_ptr<ResourceManager> makeRmWithTextures(
        std::initializer_list<const char*> names) {
        auto rm = std::make_shared<ResourceManager>();
        for (const auto* n : names) {
            rm->addTexture(n, std::make_shared<TextureManager>());
        }
        return rm;
    }

    // Mock feature pro test custom factory registrace - dynamic_pointer_cast
    // pak ověří, že create() vrátil instanci téhle třídy.
    class MockFeature final : public Feature::IMaterialFeature {
    public:
        explicit MockFeature(int payload) : payload(payload) {}
        [[nodiscard]] Manager::ShaderFeatureMask flag() const override { return 0; }
        void bind(Manager::ShaderProgram&, const Material::RenderContext&) const override {}
        [[nodiscard]] std::shared_ptr<IMaterialFeature> clone() const override {
            return std::make_shared<MockFeature>(payload);
        }
        int payload;
    };
}

TEST_CASE("FeatureRegistry: empty registry has no types") {
    const FeatureRegistry reg;
    CHECK_FALSE(reg.has("albedo"));
    CHECK_FALSE(reg.has("normalMap"));
    CHECK_FALSE(reg.has("anything"));
}

TEST_CASE("FeatureRegistry: create() on empty registry returns nullptr") {
    const FeatureRegistry reg;
    const auto rm = std::make_shared<ResourceManager>();
    const nlohmann::json entry = {{"type", "albedo"}};

    std::shared_ptr<Feature::IMaterialFeature> result;
    REQUIRE_NOTHROW(result = reg.create("albedo", entry, *rm));
    CHECK(result == nullptr);
}

TEST_CASE("FeatureRegistry: registerBuiltinFeatures populates all 7 types") {
    FeatureRegistry reg;
    registerBuiltinFeatures(reg);

    CHECK(reg.has("albedo"));
    CHECK(reg.has("normalMap"));
    CHECK(reg.has("specular"));
    CHECK(reg.has("pbr"));
    CHECK(reg.has("uvTransform"));
    CHECK(reg.has("bones"));
    CHECK(reg.has("ibl"));

    // Runtime-wired typy registry NEZNÁ - ty řeší MaterialLoader sám.
    CHECK_FALSE(reg.has("lighting"));
    CHECK_FALSE(reg.has("shadow"));
    CHECK_FALSE(reg.has("fog"));
}

TEST_CASE("FeatureRegistry: built-in albedo factory round-trip") {
    FeatureRegistry reg;
    registerBuiltinFeatures(reg);
    const auto rm = std::make_shared<ResourceManager>();
    const nlohmann::json entry = {
        {"type", "albedo"},
        {"color", {0.88f, 0.05f, 0.05f}},
        {"alpha", 0.5f},
        {"ambientIntensity", 1.5f},
    };

    const auto feature = reg.create("albedo", entry, *rm);
    REQUIRE(feature != nullptr);
    const auto albedo = std::dynamic_pointer_cast<Feature::AlbedoFeature>(feature);
    REQUIRE(albedo != nullptr);
    REQUIRE(albedo->getColor().has_value());
    CHECK(albedo->getColor()->r == Catch::Approx(0.88f));
    CHECK(albedo->getAlpha() == Catch::Approx(0.5f));
    CHECK(albedo->getAmbientIntensity() == Catch::Approx(1.5f));
}

TEST_CASE("FeatureRegistry: built-in normalMap factory resolves texture") {
    FeatureRegistry reg;
    registerBuiltinFeatures(reg);
    const auto rm = makeRmWithTextures({"barrel_normal.png"});
    const nlohmann::json entry = {{"type", "normalMap"}, {"texture", "barrel_normal.png"}};

    const auto feature = reg.create("normalMap", entry, *rm);
    REQUIRE(feature != nullptr);
    const auto nm = std::dynamic_pointer_cast<Feature::NormalMapFeature>(feature);
    REQUIRE(nm != nullptr);
    CHECK(nm->getTexture().get() == rm->getTexture("barrel_normal.png").get());
}

TEST_CASE("FeatureRegistry: built-in specular factory") {
    FeatureRegistry reg;
    registerBuiltinFeatures(reg);
    const auto rm = makeRmWithTextures({"s.png"});
    const nlohmann::json entry = {{"type", "specular"}, {"texture", "s.png"}, {"shininess", 64.0f}};

    const auto feature = reg.create("specular", entry, *rm);
    REQUIRE(feature != nullptr);
    CHECK(std::dynamic_pointer_cast<Feature::SpecularFeature>(feature) != nullptr);
}

TEST_CASE("FeatureRegistry: built-in pbr factory") {
    FeatureRegistry reg;
    registerBuiltinFeatures(reg);
    const auto rm = makeRmWithTextures({"m.png", "r.png", "ao.png"});
    const nlohmann::json entry = {
        {"type", "pbr"},
        {"metalness", "m.png"},
        {"roughness", "r.png"},
        {"aoMap", "ao.png"},
    };

    const auto feature = reg.create("pbr", entry, *rm);
    REQUIRE(feature != nullptr);
    const auto pbr = std::dynamic_pointer_cast<Feature::PbrFeature>(feature);
    REQUIRE(pbr != nullptr);
    CHECK(pbr->getMetalness().get() == rm->getTexture("m.png").get());
    CHECK(pbr->getRoughness().get() == rm->getTexture("r.png").get());
    CHECK(pbr->getAoMap().get() == rm->getTexture("ao.png").get());
}

TEST_CASE("FeatureRegistry: built-in uvTransform factory") {
    FeatureRegistry reg;
    registerBuiltinFeatures(reg);
    const auto rm = std::make_shared<ResourceManager>();
    const nlohmann::json entry = {
        {"type", "uvTransform"},
        {"scale", {2.0f, 3.0f}},
        {"offset", {0.25f, 0.5f}},
    };

    const auto feature = reg.create("uvTransform", entry, *rm);
    REQUIRE(feature != nullptr);
    const auto uv = std::dynamic_pointer_cast<Feature::UvTransformFeature>(feature);
    REQUIRE(uv != nullptr);
    CHECK(uv->getScale().x == Catch::Approx(2.0f));
    CHECK(uv->getOffset().y == Catch::Approx(0.5f));
}

TEST_CASE("FeatureRegistry: built-in bones factory") {
    FeatureRegistry reg;
    registerBuiltinFeatures(reg);
    const auto rm = std::make_shared<ResourceManager>();
    const nlohmann::json entry = {{"type", "bones"}, {"useBones", true}};

    const auto feature = reg.create("bones", entry, *rm);
    REQUIRE(feature != nullptr);
    const auto bf = std::dynamic_pointer_cast<Feature::BonesFeature>(feature);
    REQUIRE(bf != nullptr);
    CHECK(bf->getUseBones());
}

TEST_CASE("FeatureRegistry: built-in ibl factory") {
    FeatureRegistry reg;
    registerBuiltinFeatures(reg);
    const auto rm = makeRmWithTextures({"env.hdr"});
    const nlohmann::json entry = {{"type", "ibl"}, {"texture", "env.hdr"}};

    const auto feature = reg.create("ibl", entry, *rm);
    REQUIRE(feature != nullptr);
    const auto ibl = std::dynamic_pointer_cast<Feature::IblFeature>(feature);
    REQUIRE(ibl != nullptr);
    CHECK(ibl->getEnvironmentMap().get() == rm->getTexture("env.hdr").get());
}

TEST_CASE("FeatureRegistry: custom factory registers and creates instance") {
    FeatureRegistry reg;
    reg.registerFeature("custom",
        [](const nlohmann::json& entry, const Manager::ResourceManager&) {
            const int p = entry.contains("payload") ? entry.at("payload").get<int>() : 0;
            return std::static_pointer_cast<Feature::IMaterialFeature>(
                std::make_shared<MockFeature>(p));
        });

    CHECK(reg.has("custom"));

    const auto rm = std::make_shared<ResourceManager>();
    const nlohmann::json entry = {{"type", "custom"}, {"payload", 42}};

    const auto feature = reg.create("custom", entry, *rm);
    REQUIRE(feature != nullptr);
    const auto mock = std::dynamic_pointer_cast<MockFeature>(feature);
    REQUIRE(mock != nullptr);
    CHECK(mock->payload == 42);
}

TEST_CASE("FeatureRegistry: unknown type returns nullptr without crash") {
    FeatureRegistry reg;
    registerBuiltinFeatures(reg);
    const auto rm = std::make_shared<ResourceManager>();
    const nlohmann::json entry = {{"type", "nonexistent"}};

    std::shared_ptr<Feature::IMaterialFeature> result;
    REQUIRE_NOTHROW(result = reg.create("nonexistent", entry, *rm));
    CHECK(result == nullptr);
}

TEST_CASE("FeatureRegistry: loadFromJson uses custom registry with built-ins + custom factory") {
    FeatureRegistry reg;
    registerBuiltinFeatures(reg);
    reg.registerFeature("mock",
        [](const nlohmann::json&, const Manager::ResourceManager&) {
            return std::static_pointer_cast<Feature::IMaterialFeature>(
                std::make_shared<MockFeature>(7));
        });

    const auto rm = std::make_shared<ResourceManager>();
    const nlohmann::json j = {
        {"master", "basicShader"},
        {"features", {
            {{"type", "albedo"}, {"color", {1.0f, 0.0f, 0.0f}}},
            {{"type", "mock"}},
        }},
    };

    const MaterialSpec spec = loadFromJson(j, *rm, &reg);
    REQUIRE(spec.builder.featuresView().size() == 2);
    CHECK(std::dynamic_pointer_cast<Feature::AlbedoFeature>(spec.builder.featuresView()[0]) != nullptr);
    const auto mock = std::dynamic_pointer_cast<MockFeature>(spec.builder.featuresView()[1]);
    REQUIRE(mock != nullptr);
    CHECK(mock->payload == 7);
}

TEST_CASE("FeatureRegistry: loadFromJson with nullptr registry skips all static features") {
    // Edge case: pokud někdo zavolá overload s nullptr (test code, který si vyrobí
    // bare ResourceManager bez auto-bootstrap), runtime-wired flagy se nastaví,
    // ale static features se zalogují jako neznámé a přeskočí.
    const auto rm = std::make_shared<ResourceManager>();
    const nlohmann::json j = {
        {"master", "basicShader"},
        {"features", {
            {{"type", "lighting"}},
            {{"type", "albedo"}, {"color", {1.0f, 0.0f, 0.0f}}},
        }},
    };

    const MaterialSpec spec = loadFromJson(j, *rm, nullptr);
    CHECK(spec.hasLighting);
    CHECK(spec.builder.featuresView().empty());
}

TEST_CASE("FeatureRegistry: ResourceManager ctor auto-registers built-ins") {
    const ResourceManager rm;
    const auto reg = rm.getFeatureRegistry();
    REQUIRE(reg != nullptr);
    CHECK(reg->has("albedo"));
    CHECK(reg->has("normalMap"));
    CHECK(reg->has("specular"));
    CHECK(reg->has("pbr"));
    CHECK(reg->has("uvTransform"));
    CHECK(reg->has("bones"));
    CHECK(reg->has("ibl"));
}
