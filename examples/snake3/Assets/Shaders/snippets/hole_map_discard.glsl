// Hole map discard snippet - injected at @MATERIAL_FRAGMENT_PRE by
// HoleMapFeature. Discards the fragment when the greyscale hole texture
// marks the cell as a hole (r > 0.5). Uniforms hasHoleMap + holeMap are
// declared in basic.fs (they're cheap bool/sampler so we keep them in the
// master shader regardless of whether the feature is active).
if (hasHoleMap) {
    vec2 cellUV = outUvScale.x > 0.0 && outUvScale.y > 0.0
        ? TexCoords / outUvScale
        : TexCoords;
    vec2 sampleUV = vec2(cellUV.x, 1.0 - cellUV.y);
    if (texture(holeMap, sampleUV).r > 0.5) {
        discard;
    }
}
