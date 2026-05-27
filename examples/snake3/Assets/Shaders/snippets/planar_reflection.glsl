// Planar reflection snippet - injected into main3D/basic.fs at the
// @MATERIAL_FRAGMENT_POST marker by PlanarReflectionFeature. Mixes the
// pre-rendered reflection texture into the final fragment colour.
//
// Expects in scope (provided by basic.fs):
//   - material_reflectionEnable (MaterialData UBO, material_data.glsl)
//   - clipSpacePos (vertex shader output)
//   - FragColor (out vec4 from earlier in main())
//   - calcReflexion(...) (defined in functions/reflection.glsl)
//   - material_rainDropEnable + rippleOffset (UBO + basic.fs local;
//     rainDrop is toggled at runtime, so a runtime branch here keeps the
//     no-rain materials identical to the previous behaviour)
if (material_reflectionEnable != 0) {
    vec4 distortedClipPos = clipSpacePos;
    if (material_rainDropEnable != 0) {
        // Planární reflexe se rozvlní
        distortedClipPos.xy += rippleOffset * 0.05;
    }
    FragColor = vec4(calcReflexion(clipSpacePos, FragColor.rgb), FragColor.a);
}
