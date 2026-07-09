#version 330 core

// Translucent annulus (engine selection ring). vLocal is the plane's local XZ in
// [-1,1]; the ring is drawn between inner..outer radius with soft edges and a slow
// pulse. Color + pulse are driven by SelectionRingNode3D's ShaderMaterial.
in vec2 vLocal;

out vec4 FragColor;

uniform vec3 color = vec3(0.20, 1.0, 0.35); // friendly green (bright, high-contrast)
uniform float pulse = 1.0;                  // 0..1, animated by the material

void main() {
    float d = length(vLocal);             // 0 at center, 1 at plane edge
    const float inner = 0.66;
    const float outer = 0.96;
    if (d < inner || d > outer) discard;

    float edge = smoothstep(outer, outer - 0.10, d) * smoothstep(inner, inner + 0.10, d);
    float alpha = (0.85 + 0.15 * pulse) * edge; // near-opaque so it reads on terrain
    FragColor = vec4(color, alpha);
}
