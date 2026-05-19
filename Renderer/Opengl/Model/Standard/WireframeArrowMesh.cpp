#include "WireframeArrowMesh.h"

namespace Model {
    WireframeArrowMesh::WireframeArrowMesh(shared_ptr<ShaderProgram> baseShader)
        : StandardMesh(std::move(baseShader)) {
        float totalLength = 0.5f;
        float headLength = 0.15f;

        // Poloviční tloušťky (od středu ke kraji)
        float shaftWidth = 0.01f; // Velmi tenká linka
        float headWidth = 0.06f; // Šířka křídla šipky

        // Souřadnice Z (směrem do hloubky -Z)
        float zStart = 0.0f;
        float zNeck = -totalLength + headLength;
        float zTip = -totalLength; // Špička

        std::vector<Vertex> vertices;
        std::vector<GLuint> indices;

        auto addP = [&](const float x, const float y, const float z) {
            Vertex v{};
            v.position = glm::vec3(x, y, z);
            v.color = glm::vec3(1.0f); // Barvu přepíšeme v shaderu uniformem
            vertices.push_back(v);

            return static_cast<GLuint>(vertices.size() - 1);
        };

        // Funkce pro vytvoření jedné PLOCHÉ šipky v dané rovině
        // isVertical: true = rovina YZ, false = rovina XZ
        auto createFlatArrow = [&](const bool isVertical) {
            // Pomocné proměnné pro prohození os
            auto vec = [&](const float w, const float z) {
                return isVertical ? glm::vec3(0, w, z) : glm::vec3(w, 0, z);
            };

            // 1. Zadní část (Start)
            GLuint backPos = addP(vec(shaftWidth, zStart).x, vec(shaftWidth, zStart).y, zStart);
            GLuint backNeg = addP(vec(-shaftWidth, zStart).x, vec(-shaftWidth, zStart).y, zStart);

            // 2. Krk (Neck) - konec dříku
            GLuint neckPosIn = addP(vec(shaftWidth, zNeck).x, vec(shaftWidth, zNeck).y, zNeck);
            GLuint neckNegIn = addP(vec(-shaftWidth, zNeck).x, vec(-shaftWidth, zNeck).y, zNeck);

            // 3. Krk (Neck) - začátek křídla (rozšíření)
            GLuint neckPosOut = addP(vec(headWidth, zNeck).x, vec(headWidth, zNeck).y, zNeck);
            GLuint neckNegOut = addP(vec(-headWidth, zNeck).x, vec(-headWidth, zNeck).y, zNeck);

            // 4. Špička (Tip)
            const GLuint tip = addP(0, 0, zTip);

            // === SPOJENÍ ČAR (Obrys) ===

            // Dřík (Shaft) - dlouhé čáry
            indices.push_back(backPos);
            indices.push_back(neckPosIn);
            indices.push_back(backNeg);
            indices.push_back(neckNegIn);

            // Zadní stěna (uzavření dříku)
            indices.push_back(backPos);
            indices.push_back(backNeg);

            // Rozšíření hlavy (schodek)
            indices.push_back(neckPosIn);
            indices.push_back(neckPosOut);
            indices.push_back(neckNegIn);
            indices.push_back(neckNegOut);

            // Šikmé hrany ke špičce
            indices.push_back(neckPosOut);
            indices.push_back(tip);
            indices.push_back(neckNegOut);
            indices.push_back(tip);
        };

        createFlatArrow(true);
        createFlatArrow(false);

        drawElement = DrawElement::Lines;
        mesh = std::make_shared<Mesh>(vertices, indices);
    }

    void WireframeArrowMesh::render(const shared_ptr<Camera> &camera, const glm::mat4 &projection, const float dt,
                                    const glm::mat4 &parentTransform, const bool shadows) const {
        baseShader->use();
        baseShader->setMat4("view", camera->getViewMatrix());
        baseShader->setMat4("projection", projection);
        baseShader->setMat4("model", parentTransform);
        baseShader->setVec3("uColor", color);

        mesh->bind();
        glDrawElements(static_cast<GLenum>(drawElement), static_cast<int>(mesh->getIndices().size()), GL_UNSIGNED_INT,
                       nullptr);
    }
} // Model
