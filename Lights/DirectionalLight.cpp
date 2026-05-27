#include "DirectionalLight.h"

namespace Lights {
    void DirectionalLight::bind(const ShaderProgram *shader) const {
        // D1.1c: pozice/směr/ambient/diffuse/specular se migrovaly do
        // FrameData UBO (slot 0), RenderManager je tam uploaduje jednou
        // za frame. Tady už zůstává jen to, co v UBO být nemůže nebo nesmí:
        //  - material.shininess: Mesa má kvírk s pow(x, 0), takže
        //    `shininess = 32` musí být per-program nastavený, jinak
        //    získáme konstantní spec a flat shading
        //  - sampler indices: GLSL samplery nesmí být uvnitř UBO bloku,
        //    musí se vždy posílat setInt na texture unit
        shader->use();
        shader->setFloat("material.shininess", shininess);
        shader->setInt("material.ambient", 0);
        shader->setInt("material.diffuse", 1);
        shader->setInt("material.specular", 2);
    }
} // Lights
