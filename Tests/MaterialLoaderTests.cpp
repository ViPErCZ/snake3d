#include <catch2/catch_all.hpp>

#include <memory>

#include <nlohmann/json.hpp>

#include "../Manager/ResourceManager.h"
#include "../Manager/TextureManager.h"
#include "../Renderer/Opengl/Material/Feature/AlbedoFeature.h"
#include "../Renderer/Opengl/Material/Feature/BonesFeature.h"
#include "../Renderer/Opengl/Material/Feature/IblFeature.h"
#include "../Renderer/Opengl/Material/Feature/NormalMapFeature.h"
#include "../Renderer/Opengl/Material/Feature/PbrFeature.h"
#include "../Renderer/Opengl/Material/Feature/SpecularFeature.h"
#include "../Renderer/Opengl/Material/Feature/UvTransformFeature.h"
#include <snake3d/Resource/MaterialLoader.h>
#include <snake3d/Tools/Blending.h>

using Manager::ResourceManager;
using Manager::TextureManager;
using Resource::MaterialSpec;
using Resource::loadFromJson;

namespace {
    // ResourceManager pre-naplněný "fake" texturami. Default-konstruovaný
    // TextureManager neudělá žádné GL volání (textures vector zůstane prázdný
    // dokud nikdo nezavolá addTexture(id)), takže ho můžeme bez problémů
    // používat v unit testu bez GL kontextu. MaterialLoader::loadFromJson
    // pouze předává shared_ptr<TextureManager> dál do Feature konstruktorů -
    // bind() není volán.
    std::shared_ptr<ResourceManager> makeRmWithTextures(
        std::initializer_list<const char*> names) {
        auto rm = std::make_shared<ResourceManager>();
        for (const auto* n : names) {
            rm->addTexture(n, std::make_shared<TextureManager>());
        }
        return rm;
    }
}

TEST_CASE("MaterialLoader: empty features array yields empty spec") {
    const auto rm = std::make_shared<ResourceManager>();
    const nlohmann::json j = {
        {"master", "basicShader"},
        {"features", nlohmann::json::array()},
    };

    const MaterialSpec spec = loadFromJson(j, *rm);

    CHECK(spec.builder.masterName() == "basicShader");
    CHECK(spec.builder.featuresView().empty());
    CHECK_FALSE(spec.hasLighting);
    CHECK_FALSE(spec.hasShadow);
    CHECK_FALSE(spec.hasFog);
    CHECK(spec.blending == Tools::Blending::Opaque);
}

TEST_CASE("MaterialLoader: runtime-wired features set flags but add nothing to builder") {
    const auto rm = std::make_shared<ResourceManager>();
    const nlohmann::json j = {
        {"master", "basicShader"},
        {"features", {
            {{"type", "lighting"}},
            {{"type", "shadow"}},
            {{"type", "fog"}},
        }},
    };

    const MaterialSpec spec = loadFromJson(j, *rm);

    CHECK(spec.hasLighting);
    CHECK(spec.hasShadow);
    CHECK(spec.hasFog);
    // Runtime-wired typy se appendují až na callsitu - builder zůstává prázdný.
    CHECK(spec.builder.featuresView().empty());
}

TEST_CASE("MaterialLoader: albedo feature with color parses into AlbedoFeature") {
    const auto rm = std::make_shared<ResourceManager>();
    const nlohmann::json j = {
        {"master", "basicShader"},
        {"features", {
            {{"type", "albedo"}, {"color", {0.88f, 0.05f, 0.05f}}},
        }},
    };

    const MaterialSpec spec = loadFromJson(j, *rm);

    REQUIRE(spec.builder.featuresView().size() == 1);
    const auto albedo = std::dynamic_pointer_cast<Feature::AlbedoFeature>(
        spec.builder.featuresView()[0]);
    REQUIRE(albedo != nullptr);

    const auto color = albedo->getColor();
    REQUIRE(color.has_value());
    CHECK(color->r == Catch::Approx(0.88f));
    CHECK(color->g == Catch::Approx(0.05f));
    CHECK(color->b == Catch::Approx(0.05f));
    // Žádná textura nebyla v JSONu - albedo by měl být nullptr (lazy pattern).
    CHECK(albedo->getAlbedo() == nullptr);
}

TEST_CASE("MaterialLoader: albedo without color leaves color unset, defaults applied") {
    const auto rm = std::make_shared<ResourceManager>();
    const nlohmann::json j = {
        {"master", "basicShader"},
        {"features", {
            {{"type", "albedo"}, {"ambientIntensity", 2.5f}},
        }},
    };

    const MaterialSpec spec = loadFromJson(j, *rm);

    REQUIRE(spec.builder.featuresView().size() == 1);
    const auto albedo = std::dynamic_pointer_cast<Feature::AlbedoFeature>(
        spec.builder.featuresView()[0]);
    REQUIRE(albedo != nullptr);
    CHECK_FALSE(albedo->getColor().has_value());
    CHECK(albedo->getAmbientIntensity() == Catch::Approx(2.5f));
    CHECK(albedo->getAlpha() == Catch::Approx(1.0f));
}

TEST_CASE("MaterialLoader: normalMap without texture key gets nullptr (lazy pattern)") {
    // BarrelNode3D pattern - NormalMapFeature(nullptr) je validní, callsite si
    // texturu doplní později po async load.
    const auto rm = std::make_shared<ResourceManager>();
    const nlohmann::json j = {
        {"master", "basicShader"},
        {"features", {
            {{"type", "normalMap"}},
        }},
    };

    const MaterialSpec spec = loadFromJson(j, *rm);

    REQUIRE(spec.builder.featuresView().size() == 1);
    const auto nm = std::dynamic_pointer_cast<Feature::NormalMapFeature>(
        spec.builder.featuresView()[0]);
    REQUIRE(nm != nullptr);
    CHECK(nm->getTexture() == nullptr);
}

TEST_CASE("MaterialLoader: normalMap with texture key resolves via ResourceManager") {
    const auto rm = makeRmWithTextures({"torch_normal.png"});
    const nlohmann::json j = {
        {"master", "basicShader"},
        {"features", {
            {{"type", "normalMap"}, {"texture", "torch_normal.png"}},
        }},
    };

    const MaterialSpec spec = loadFromJson(j, *rm);

    REQUIRE(spec.builder.featuresView().size() == 1);
    const auto nm = std::dynamic_pointer_cast<Feature::NormalMapFeature>(
        spec.builder.featuresView()[0]);
    REQUIRE(nm != nullptr);
    REQUIRE(nm->getTexture() != nullptr);
    CHECK(nm->getTexture().get() == rm->getTexture("torch_normal.png").get());
}

TEST_CASE("MaterialLoader: unknown feature type is skipped without crash") {
    const auto rm = std::make_shared<ResourceManager>();
    const nlohmann::json j = {
        {"master", "basicShader"},
        {"features", {
            {{"type", "totally_unknown_feature"}},
            {{"type", "albedo"}, {"color", {1.0f, 1.0f, 1.0f}}},
        }},
    };

    // Loader emituje std::cerr warning ale nehází - albedo se appendne, unknown se
    // přeskočí.
    MaterialSpec spec;
    REQUIRE_NOTHROW(spec = loadFromJson(j, *rm));
    CHECK(spec.builder.featuresView().size() == 1);
    const auto albedo = std::dynamic_pointer_cast<Feature::AlbedoFeature>(
        spec.builder.featuresView()[0]);
    CHECK(albedo != nullptr);
}

TEST_CASE("MaterialLoader: _comment top-level key is ignored") {
    const auto rm = std::make_shared<ResourceManager>();
    const nlohmann::json j = {
        {"_comment", "this is a doc string and must not break loading"},
        {"master", "basicShader"},
        {"features", {
            {{"type", "albedo"}, {"color", {0.5f, 0.5f, 0.5f}}},
        }},
    };

    MaterialSpec spec;
    REQUIRE_NOTHROW(spec = loadFromJson(j, *rm));
    CHECK(spec.builder.masterName() == "basicShader");
    CHECK(spec.builder.featuresView().size() == 1);
}

TEST_CASE("MaterialLoader: blending parsing") {
    const auto rm = std::make_shared<ResourceManager>();

    SECTION("translucent") {
        const nlohmann::json j = {
            {"master", "basicShader"},
            {"blending", "translucent"},
            {"features", nlohmann::json::array()},
        };
        CHECK(loadFromJson(j, *rm).blending == Tools::Blending::Translucent);
    }

    SECTION("additive") {
        const nlohmann::json j = {
            {"master", "basicShader"},
            {"blending", "additive"},
            {"features", nlohmann::json::array()},
        };
        CHECK(loadFromJson(j, *rm).blending == Tools::Blending::Additive);
    }

    SECTION("alphaAdditive") {
        const nlohmann::json j = {
            {"master", "basicShader"},
            {"blending", "alphaAdditive"},
            {"features", nlohmann::json::array()},
        };
        CHECK(loadFromJson(j, *rm).blending == Tools::Blending::AlphaAdditive);
    }

    SECTION("unknown blending falls back to opaque + emits warning") {
        const nlohmann::json j = {
            {"master", "basicShader"},
            {"blending", "garbageMode"},
            {"features", nlohmann::json::array()},
        };
        MaterialSpec spec;
        REQUIRE_NOTHROW(spec = loadFromJson(j, *rm));
        CHECK(spec.blending == Tools::Blending::Opaque);
    }

    SECTION("missing blending defaults to opaque") {
        const nlohmann::json j = {
            {"master", "basicShader"},
            {"features", nlohmann::json::array()},
        };
        CHECK(loadFromJson(j, *rm).blending == Tools::Blending::Opaque);
    }
}

TEST_CASE("MaterialLoader: missing master leaves masterName empty (defensive)") {
    // Loader záměrně neházi error - master se nastaví jen pokud klíč existuje.
    // Build pak na empty masterName selže až v ShaderRegistry, což je explicitní
    // chyba a snáz se debuguje než silent fallback.
    const auto rm = std::make_shared<ResourceManager>();
    const nlohmann::json j = {
        {"features", nlohmann::json::array()},
    };

    MaterialSpec spec;
    REQUIRE_NOTHROW(spec = loadFromJson(j, *rm));
    CHECK(spec.builder.masterName().empty());
}

TEST_CASE("MaterialLoader: specular feature uses default shininess when key omitted") {
    // Default shininess je 32.0f (viz SpecularFeature.h). SpecularFeature
    // nevystavuje getter, ale konstruktor s nullptr texturou je validní -
    // ověříme aspoň, že feature byla appendnuta a má správný dynamic type.
    const auto rm = makeRmWithTextures({"some_specular.png"});
    const nlohmann::json j = {
        {"master", "basicShader"},
        {"features", {
            {{"type", "specular"}, {"texture", "some_specular.png"}},
        }},
    };

    const MaterialSpec spec = loadFromJson(j, *rm);
    REQUIRE(spec.builder.featuresView().size() == 1);
    const auto sf = std::dynamic_pointer_cast<Feature::SpecularFeature>(
        spec.builder.featuresView()[0]);
    REQUIRE(sf != nullptr);
}

TEST_CASE("MaterialLoader: pbr feature resolves metalness/roughness/aoMap textures") {
    const auto rm = makeRmWithTextures({
        "barrel_metalness.png", "barrel_roughness.png", "barrel_ao.png"});
    const nlohmann::json j = {
        {"master", "basicShader"},
        {"features", {
            {
                {"type", "pbr"},
                {"metalness", "barrel_metalness.png"},
                {"roughness", "barrel_roughness.png"},
                {"aoMap",     "barrel_ao.png"},
            },
        }},
    };

    const MaterialSpec spec = loadFromJson(j, *rm);
    REQUIRE(spec.builder.featuresView().size() == 1);
    const auto pbr = std::dynamic_pointer_cast<Feature::PbrFeature>(
        spec.builder.featuresView()[0]);
    REQUIRE(pbr != nullptr);
    CHECK(pbr->getMetalness().get() == rm->getTexture("barrel_metalness.png").get());
    CHECK(pbr->getRoughness().get() == rm->getTexture("barrel_roughness.png").get());
    CHECK(pbr->getAoMap().get()     == rm->getTexture("barrel_ao.png").get());
}

TEST_CASE("MaterialLoader: pbr without aoMap leaves it nullptr") {
    const auto rm = makeRmWithTextures({"m.png", "r.png"});
    const nlohmann::json j = {
        {"master", "basicShader"},
        {"features", {
            {
                {"type", "pbr"},
                {"metalness", "m.png"},
                {"roughness", "r.png"},
            },
        }},
    };

    const MaterialSpec spec = loadFromJson(j, *rm);
    REQUIRE(spec.builder.featuresView().size() == 1);
    const auto pbr = std::dynamic_pointer_cast<Feature::PbrFeature>(
        spec.builder.featuresView()[0]);
    REQUIRE(pbr != nullptr);
    CHECK(pbr->getMetalness() != nullptr);
    CHECK(pbr->getRoughness() != nullptr);
    CHECK(pbr->getAoMap() == nullptr);
}

TEST_CASE("MaterialLoader: uvTransform parses scale and offset") {
    const auto rm = std::make_shared<ResourceManager>();
    const nlohmann::json j = {
        {"master", "basicShader"},
        {"features", {
            {{"type", "uvTransform"},
             {"scale",  {2.0f, 3.0f}},
             {"offset", {0.25f, 0.5f}}},
        }},
    };

    const MaterialSpec spec = loadFromJson(j, *rm);
    REQUIRE(spec.builder.featuresView().size() == 1);
    const auto uv = std::dynamic_pointer_cast<Feature::UvTransformFeature>(
        spec.builder.featuresView()[0]);
    REQUIRE(uv != nullptr);
    CHECK(uv->getScale().x  == Catch::Approx(2.0f));
    CHECK(uv->getScale().y  == Catch::Approx(3.0f));
    CHECK(uv->getOffset().x == Catch::Approx(0.25f));
    CHECK(uv->getOffset().y == Catch::Approx(0.5f));
}

TEST_CASE("MaterialLoader: uvTransform defaults to identity when keys omitted") {
    const auto rm = std::make_shared<ResourceManager>();
    const nlohmann::json j = {
        {"master", "basicShader"},
        {"features", {
            {{"type", "uvTransform"}},
        }},
    };

    const MaterialSpec spec = loadFromJson(j, *rm);
    REQUIRE(spec.builder.featuresView().size() == 1);
    const auto uv = std::dynamic_pointer_cast<Feature::UvTransformFeature>(
        spec.builder.featuresView()[0]);
    REQUIRE(uv != nullptr);
    CHECK(uv->getScale().x  == Catch::Approx(1.0f));
    CHECK(uv->getScale().y  == Catch::Approx(1.0f));
    CHECK(uv->getOffset().x == Catch::Approx(0.0f));
    CHECK(uv->getOffset().y == Catch::Approx(0.0f));
}

TEST_CASE("MaterialLoader: bones feature parses useBones flag") {
    const auto rm = std::make_shared<ResourceManager>();

    SECTION("useBones=true") {
        const nlohmann::json j = {
            {"master", "basicShader"},
            {"features", {
                {{"type", "bones"}, {"useBones", true}},
            }},
        };
        const auto spec = loadFromJson(j, *rm);
        REQUIRE(spec.builder.featuresView().size() == 1);
        const auto bf = std::dynamic_pointer_cast<Feature::BonesFeature>(
            spec.builder.featuresView()[0]);
        REQUIRE(bf != nullptr);
        CHECK(bf->getUseBones());
    }

    SECTION("useBones omitted defaults to false") {
        const nlohmann::json j = {
            {"master", "basicShader"},
            {"features", {
                {{"type", "bones"}},
            }},
        };
        const auto spec = loadFromJson(j, *rm);
        REQUIRE(spec.builder.featuresView().size() == 1);
        const auto bf = std::dynamic_pointer_cast<Feature::BonesFeature>(
            spec.builder.featuresView()[0]);
        REQUIRE(bf != nullptr);
        CHECK_FALSE(bf->getUseBones());
    }
}

TEST_CASE("MaterialLoader: ibl feature resolves environment map texture") {
    const auto rm = makeRmWithTextures({"sky_env.hdr"});
    const nlohmann::json j = {
        {"master", "basicShader"},
        {"features", {
            {{"type", "ibl"}, {"texture", "sky_env.hdr"}},
        }},
    };

    const MaterialSpec spec = loadFromJson(j, *rm);
    REQUIRE(spec.builder.featuresView().size() == 1);
    const auto ibl = std::dynamic_pointer_cast<Feature::IblFeature>(
        spec.builder.featuresView()[0]);
    REQUIRE(ibl != nullptr);
    CHECK(ibl->getEnvironmentMap().get() == rm->getTexture("sky_env.hdr").get());
}

TEST_CASE("MaterialLoader: feature entry missing 'type' is skipped") {
    const auto rm = std::make_shared<ResourceManager>();
    const nlohmann::json j = {
        {"master", "basicShader"},
        {"features", {
            {{"color", {1.0f, 0.0f, 0.0f}}},   // chybí "type"
            {{"type", "albedo"}, {"color", {0.0f, 1.0f, 0.0f}}},
        }},
    };

    MaterialSpec spec;
    REQUIRE_NOTHROW(spec = loadFromJson(j, *rm));
    REQUIRE(spec.builder.featuresView().size() == 1);
    const auto albedo = std::dynamic_pointer_cast<Feature::AlbedoFeature>(
        spec.builder.featuresView()[0]);
    REQUIRE(albedo != nullptr);
    CHECK(albedo->getColor()->g == Catch::Approx(1.0f));
}

TEST_CASE("MaterialLoader: pilot material snake_tile.json shape (in-memory)") {
    // Mirror snake_tile.json struktury jako sanity check: 3 runtime flagy + 1
    // statická AlbedoFeature s červenou barvou v insertion order.
    const auto rm = std::make_shared<ResourceManager>();
    const nlohmann::json j = {
        {"master", "basicShader"},
        {"blending", "opaque"},
        {"features", {
            {{"type", "lighting"}},
            {{"type", "shadow"}},
            {{"type", "albedo"}, {"color", {0.88f, 0.05f, 0.05f}}},
            {{"type", "fog"}},
        }},
    };

    const MaterialSpec spec = loadFromJson(j, *rm);
    CHECK(spec.hasLighting);
    CHECK(spec.hasShadow);
    CHECK(spec.hasFog);
    CHECK(spec.blending == Tools::Blending::Opaque);
    REQUIRE(spec.builder.featuresView().size() == 1);
    const auto albedo = std::dynamic_pointer_cast<Feature::AlbedoFeature>(
        spec.builder.featuresView()[0]);
    REQUIRE(albedo != nullptr);
    REQUIRE(albedo->getColor().has_value());
    CHECK(albedo->getColor()->r == Catch::Approx(0.88f));
}
