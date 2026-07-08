#ifndef SNAKE3_TEXTURESLOTS_H
#define SNAKE3_TEXTURESLOTS_H

namespace Material::TextureSlots {
    // Centrální tabulka texturních jednotek pro 3D pipelinu (main3D /
    // basicShader rodina). Každá feature ví, na který slot bindovat.
    //
    // Hodnoty zrcadlí StandardMaterial::bind (B3a era). Po dokončení B fáze
    // budou všechna pevná čísla jen tady - feature classes přečtou z této
    // tabulky a kolizní detekce je triviální (viz unique values check).
    constexpr int Albedo           = 0;  // material.ambient (legacy name)
    constexpr int Normal           = 1;  // material.diffuse (legacy name)
    constexpr int Specular         = 2;  // material.specular
    constexpr int ShadowArray      = 3;  // shadowMap (sampler2DArray, cascades)
    constexpr int Metalness        = 4;
    constexpr int Roughness        = 5;
    constexpr int EnvironmentMap   = 6;  // IBL cubemap
    constexpr int AoMap            = 7;  // ambient occlusion
    constexpr int HoleMap          = 8;
    constexpr int PlanarReflection = 20; // reflection texture (far slot)
    constexpr int Refraction       = 21; // refraction texture (scene-minus-water, for water)
} // Material::TextureSlots

#endif //SNAKE3_TEXTURESLOTS_H
