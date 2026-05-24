// Planar reflection snippet - injected into main3D/basic.fs at the
// @MATERIAL_FRAGMENT_POST marker by PlanarReflectionFeature. Mixes the
// pre-rendered reflection texture into the final fragment colour.
//
// Expects in scope (provided by basic.fs):
//   - reflectionEnable (uniform bool, declared in functions/reflection.glsl)
//   - clipSpacePos (vertex shader output)
//   - FragColor (out vec4 from earlier in main())
//   - calcReflexion(...) (defined in functions/reflection.glsl)
//   - rainDropEnable + rippleOffset (declared in basic.fs; rainDrop is
//     toggled at runtime, so a runtime branch here keeps the no-rain
//     materials identical to the previous behaviour)
if (reflectionEnable) {
    vec4 distortedClipPos = clipSpacePos;
    if (rainDropEnable) {
        // Planární reflexe se rozvlní
        distortedClipPos.xy += rippleOffset * 0.05;
    }
    FragColor = vec4(calcReflexion(clipSpacePos, FragColor.rgb), FragColor.a);
}
