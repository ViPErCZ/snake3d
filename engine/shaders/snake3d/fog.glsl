// Engine distance fog (linear). Opt-in: any fragment shader can
//   #include "snake3d/fog.glsl"
// and call applyFog(color, distance) to blend toward a fog color with camera
// distance. The uniforms carry sane overcast defaults so it works with no C++
// wiring; a game can override them (uFogColor / uFogStart / uFogEnd) per program.
//
// Lives engine-side (resolved from the EngineShaders include root) so every
// example gets consistent distance fog without copying GLSL.
#ifndef SNAKE3D_FOG_GLSL
#define SNAKE3D_FOG_GLSL

uniform vec3  uFogColor = vec3(0.56, 0.59, 0.62);
uniform float uFogStart = 55.0;
uniform float uFogEnd   = 280.0;
uniform float uFogEnabled = 1.0; // 0 disables (mix factor forced to 0)

// Blend `color` toward uFogColor by linear distance between uFogStart..uFogEnd.
vec3 applyFog(vec3 color, float camDist) {
    float denom = max(uFogEnd - uFogStart, 0.001);
    float f = clamp((camDist - uFogStart) / denom, 0.0, 1.0) * clamp(uFogEnabled, 0.0, 1.0);
    return mix(color, uFogColor, f);
}

#endif // SNAKE3D_FOG_GLSL
