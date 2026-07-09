if (material_hasHoleMap != 0) {
    vec2 cellUV = outUvScale.x > 0.0 && outUvScale.y > 0.0
        ? TexCoords / outUvScale
        : TexCoords;
    vec2 sampleUV = vec2(cellUV.x, 1.0 - cellUV.y);
    if (texture(holeMap, sampleUV).r > 0.5) {
        discard;
    }
}
