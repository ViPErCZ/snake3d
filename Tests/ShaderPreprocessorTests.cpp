#include <catch2/catch_all.hpp>

#include <snake3d/Resource/ShaderPreprocessor.h>

using Resource::ShaderPreprocessor;
using Resource::shader_placeholder_error;
using Resource::MarkerLocation;

TEST_CASE("ShaderPreprocessor::injectDefines inserts after #version") {
    const std::string source =
        "#version 330 core\n"
        "uniform float a;\n"
        "void main() {}\n";

    const auto out = ShaderPreprocessor::injectDefines(source, {"FOO", "BAR"});

    const auto versionPos = out.find("#version 330 core");
    const auto fooPos = out.find("#define FOO");
    const auto barPos = out.find("#define BAR");
    const auto uniformPos = out.find("uniform float a;");

    REQUIRE(versionPos != std::string::npos);
    REQUIRE(fooPos != std::string::npos);
    REQUIRE(barPos != std::string::npos);
    REQUIRE(uniformPos != std::string::npos);

    CHECK(versionPos < fooPos);
    CHECK(fooPos < barPos);
    CHECK(barPos < uniformPos);
}

TEST_CASE("ShaderPreprocessor::injectDefines no-op for empty list") {
    const std::string source = "#version 330 core\nvoid main() {}\n";
    CHECK(ShaderPreprocessor::injectDefines(source, {}) == source);
}

TEST_CASE("ShaderPreprocessor::injectDefines without #version prepends") {
    const std::string source = "void main() {}\n";
    const auto out = ShaderPreprocessor::injectDefines(source, {"FOO"});
    CHECK(out.starts_with("#define FOO\n"));
    CHECK(out.find("void main()") != std::string::npos);
}

TEST_CASE("ShaderPreprocessor::injectSnippet replaces marker line") {
    const std::string source =
        "void main() {\n"
        "    // @SLOT_X\n"
        "    float a = 0.0;\n"
        "}\n";

    const auto out = ShaderPreprocessor::injectSnippet(source, "@SLOT_X", "if (x) discard;");

    CHECK(out.find("// @SLOT_X") == std::string::npos);
    CHECK(out.find("if (x) discard;") != std::string::npos);
    CHECK(out.find("float a = 0.0;") != std::string::npos);
}

TEST_CASE("ShaderPreprocessor::injectSnippet missing marker is no-op") {
    const std::string source = "void main() {}\n";
    CHECK(ShaderPreprocessor::injectSnippet(source, "@NOTHERE", "anything") == source);
}

TEST_CASE("ShaderPreprocessor::injectSnippet empty snippet is no-op") {
    const std::string source = "// @SLOT_X\n";
    CHECK(ShaderPreprocessor::injectSnippet(source, "@SLOT_X", "") == source);
}

TEST_CASE("ShaderPreprocessor::injectSnippet replaces only first occurrence") {
    const std::string source =
        "// @SLOT_X\n"
        "middle\n"
        "// @SLOT_X\n";

    const auto out = ShaderPreprocessor::injectSnippet(source, "@SLOT_X", "REPLACED");

    // First marker replaced, second remains untouched.
    CHECK(out.find("REPLACED") != std::string::npos);
    const auto stillThere = out.find("// @SLOT_X");
    CHECK(stillThere != std::string::npos);
    // And after the surviving marker there shouldn't be another REPLACED.
    CHECK(out.find("REPLACED", stillThere) == std::string::npos);
}

TEST_CASE("ShaderPreprocessor::injectSnippet appends newline if snippet missing one") {
    const std::string source = "// @SLOT_X\nafter\n";
    const auto out = ShaderPreprocessor::injectSnippet(source, "@SLOT_X", "code;");
    // Result should be "code;\nafter\n", not "code;after\n".
    CHECK(out == "code;\nafter\n");
}

// ---- C1: material placeholder validation -------------------------------

TEST_CASE("scanMaterialMarkers finds @MATERIAL_ tokens with 1-based line numbers") {
    const std::string source =
        "void main() {\n"
        "    // @MATERIAL_FRAGMENT_PRE\n"
        "    float a = 0.0;\n"
        "    // @MATERIAL_FRAGMENT_POST\n"
        "}\n";

    const auto markers = ShaderPreprocessor::scanMaterialMarkers(source);
    REQUIRE(markers.size() == 2);
    CHECK(markers[0].name == "@MATERIAL_FRAGMENT_PRE");
    CHECK(markers[0].line == 2);
    CHECK(markers[1].name == "@MATERIAL_FRAGMENT_POST");
    CHECK(markers[1].line == 4);
}

TEST_CASE("scanMaterialMarkers ignores non-MATERIAL @ tokens") {
    const std::string source =
        "@OTHER_THING\n"
        "// @MATERIAL_FRAGMENT_PRE\n"
        "@MATERIAL_BUT_NOT_WHITELISTED\n";

    const auto markers = ShaderPreprocessor::scanMaterialMarkers(source);
    // @OTHER_THING isn't @MATERIAL_-prefixed → ignored.
    // @MATERIAL_BUT_NOT_WHITELISTED has the prefix → scanned (whitelist check
    // happens elsewhere, scanner is purely syntactic).
    REQUIRE(markers.size() == 2);
    CHECK(markers[0].name == "@MATERIAL_FRAGMENT_PRE");
    CHECK(markers[1].name == "@MATERIAL_BUT_NOT_WHITELISTED");
}

TEST_CASE("validateUniqueMarkers throws on duplicates") {
    std::vector<MarkerLocation> markers = {
        {"@MATERIAL_FRAGMENT_PRE", 5},
        {"@MATERIAL_FRAGMENT_PRE", 12},
    };
    CHECK_THROWS_AS(
        ShaderPreprocessor::validateUniqueMarkers(markers, "test.fs"),
        shader_placeholder_error
    );
}

TEST_CASE("validateUniqueMarkers passes for distinct markers") {
    std::vector<MarkerLocation> markers = {
        {"@MATERIAL_FRAGMENT_PRE", 5},
        {"@MATERIAL_FRAGMENT_POST", 12},
    };
    CHECK_NOTHROW(ShaderPreprocessor::validateUniqueMarkers(markers, "test.fs"));
}

TEST_CASE("validateSnippetsMatch throws when snippet has no matching marker in source") {
    std::map<std::string, std::string> snippets = {
        {"@MATERIAL_FRAGMENT_POST", "do_stuff();"},
    };
    std::set<std::string> present = {"@MATERIAL_FRAGMENT_PRE"};
    CHECK_THROWS_AS(
        ShaderPreprocessor::validateSnippetsMatch(snippets, present),
        shader_placeholder_error
    );
}

TEST_CASE("validateSnippetsMatch passes when every snippet has a target marker") {
    std::map<std::string, std::string> snippets = {
        {"@MATERIAL_FRAGMENT_PRE", "pre();"},
        {"@MATERIAL_FRAGMENT_POST", "post();"},
    };
    std::set<std::string> present = {"@MATERIAL_FRAGMENT_PRE", "@MATERIAL_FRAGMENT_POST"};
    CHECK_NOTHROW(ShaderPreprocessor::validateSnippetsMatch(snippets, present));
}

TEST_CASE("applySnippets replaces marker lines with snippet content") {
    const std::string source =
        "void main() {\n"
        "    // @MATERIAL_FRAGMENT_PRE\n"
        "    float a = 0.0;\n"
        "}\n";
    std::map<std::string, std::string> snippets = {
        {"@MATERIAL_FRAGMENT_PRE", "float pre = 1.0;"},
    };
    const auto out = ShaderPreprocessor::applySnippets(source, snippets);
    CHECK(out.find("// @MATERIAL_FRAGMENT_PRE") == std::string::npos);
    CHECK(out.find("float pre = 1.0;") != std::string::npos);
}

TEST_CASE("applySnippets leaves marker lines untouched when no snippet provided") {
    const std::string source = "// @MATERIAL_FRAGMENT_POST\n";
    // Empty map => no replacements.
    const auto out = ShaderPreprocessor::applySnippets(source, {});
    CHECK(out == source);
}

TEST_CASE("processMaterialPlaceholders happy path") {
    const std::string source =
        "void main() {\n"
        "    // @MATERIAL_FRAGMENT_PRE\n"
        "    // @MATERIAL_FRAGMENT_POST\n"
        "}\n";
    std::map<std::string, std::string> snippets = {
        {"@MATERIAL_FRAGMENT_PRE", "pre();"},
        {"@MATERIAL_FRAGMENT_POST", "post();"},
    };
    const auto out = ShaderPreprocessor::processMaterialPlaceholders(source, "test.fs", snippets);
    CHECK(out.find("pre();") != std::string::npos);
    CHECK(out.find("post();") != std::string::npos);
}

TEST_CASE("processMaterialPlaceholders throws on duplicate marker in source") {
    const std::string source =
        "// @MATERIAL_FRAGMENT_PRE\n"
        "// @MATERIAL_FRAGMENT_PRE\n";
    CHECK_THROWS_AS(
        ShaderPreprocessor::processMaterialPlaceholders(source, "test.fs", {}),
        shader_placeholder_error
    );
}

TEST_CASE("processMaterialPlaceholders throws when feature provides marker not in source") {
    const std::string source = "void main() {}\n";
    std::map<std::string, std::string> snippets = {
        {"@MATERIAL_FRAGMENT_PRE", "pre();"},
    };
    CHECK_THROWS_AS(
        ShaderPreprocessor::processMaterialPlaceholders(source, "test.fs", snippets),
        shader_placeholder_error
    );
}
