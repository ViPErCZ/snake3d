#include "LightNode3D.h"

namespace Model {
    LightNode3D::LightNode3D(const shared_ptr<ShaderProgram> &baseShader) {
        arrowMesh = make_shared<WireframeArrowMesh>(baseShader);
    }

    glm::mat4 LightNode3D::calculateArrowTransform(const glm::vec3 position, const glm::vec3 direction) {
        // 1. Normalizovat cílový směr (kam to má svítit)
        const glm::vec3 destDir = glm::normalize(direction);

        // 2. Definovat počáteční směr meshe (Model Space)
        // Protože jsi mesh vygeneroval podél osy -Z (od 0 do -length),
        // musíš algoritmu říct, že "přirozený" směr šipky je (0, 0, -1).
        constexpr auto srcDir = glm::vec3(0, 0, -1);

        // 3. Spočítat osu rotace a úhel
        const glm::vec3 rotAxis = glm::cross(srcDir, destDir);
        const float dotProduct = glm::dot(srcDir, destDir);

        glm::mat4 rotationMatrix(1.0f);

        // Ošetření hraničních případů (paralelní vektory)
        // Pokud je dotProduct blízko 1 nebo -1, cross product je blízko 0.
        if (glm::length(rotAxis) < 0.001f) {
            // Vektory jsou rovnoběžné
            if (dotProduct < 0.0f) {
                // Jsou proti sobě (180 stupňů) -> Světlo svítí přesně dozadu (+Z)
                // Musíme to otočit o 180. Je jedno kolem jaké osy, třeba X nebo Y.
                rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(0, 1, 0));
            }
            // Pokud dotProduct > 0, jsou shodné -> Žádná rotace (identity matrix)
        } else {
            // Matematika pro Quaternion z dvou vektorů
            // s = 2 * cos(theta/2)
            const float s = sqrt((1.0f + dotProduct) * 2.0f);
            const float invs = 1.0f / s;

            glm::quat q;
            q.x = rotAxis.x * invs;
            q.y = rotAxis.y * invs;
            q.z = rotAxis.z * invs;
            q.w = s * 0.5f;

            rotationMatrix = glm::mat4_cast(q);
        }

        // 4. Sestavení finální Model Matrix
        auto model = glm::mat4(1.0f);
        model = glm::translate(model, position); // Nejdřív posun
        model = model * rotationMatrix; // Pak rotace

        return model;
    }
} // Model
