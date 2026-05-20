#ifndef SNAKE3_MATERIALPLACEHOLDER_H
#define SNAKE3_MATERIALPLACEHOLDER_H

#include <array>
#include <string_view>

namespace Manager {
    // C3: Strongly-typed enum nahrazující stringly-typed `@MATERIAL_*` markery
    // v feature API. Features dnes deklarují `snippetPaths()` jako
    // `map<MaterialSlot, string>` - kompilátor chytí překlepy slotu, IDE
    // šeptá auto-complete. Mapování enum ↔ string marker je v slotMarker().
    //
    // Master shader pořád obsahuje stringové komentáře (`// @MATERIAL_*`),
    // ShaderPreprocessor::scanMaterialMarkers vrací string názvy. Bridge mezi
    // typed (feature side) a string (shader side) je v MaterialBuilder.
    //
    // Přidání nového slotu = jedna entry v enumu + odpovídající case v
    // slotMarker() + entry v kAllSlots + komentář v master shaderu + feature
    // který slot konzumuje.
    enum class MaterialSlot {
        FragmentPre,
        FragmentPost,
    };

    inline constexpr std::array<MaterialSlot, 2> kAllSlots = {
        MaterialSlot::FragmentPre,
        MaterialSlot::FragmentPost,
    };

    [[nodiscard]] constexpr std::string_view slotMarker(MaterialSlot slot) {
        switch (slot) {
            case MaterialSlot::FragmentPre:  return "@MATERIAL_FRAGMENT_PRE";
            case MaterialSlot::FragmentPost: return "@MATERIAL_FRAGMENT_POST";
        }
        return "";
    }

    // Lookup string marker → slot enum. Vrací prázdný optional, pokud marker
    // není v žádném slotu. Použité ShaderPreprocessor warnings + bridge cestou.
    [[nodiscard]] bool isKnownMaterialPlaceholder(std::string_view marker);
} // Manager

#endif //SNAKE3_MATERIALPLACEHOLDER_H
