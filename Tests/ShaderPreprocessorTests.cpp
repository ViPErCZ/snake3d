#include <catch2/catch_all.hpp>

#include "../Resource/ShaderPreprocessor.h"

using Resource::ShaderPreprocessor;

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
