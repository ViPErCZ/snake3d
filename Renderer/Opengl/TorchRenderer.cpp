#include "TorchRenderer.h"

using namespace ItemsDto;
using namespace Manager;

namespace Renderer {
    TorchRenderer::TorchRenderer(Cube *cube, Camera *camera, const glm::mat4 &projection,
                                 ResourceManager *resManager): cube(cube), camera(camera),
                                                               projection(projection),
                                                               resourceManager(resManager) {
        mesh = resourceManager->getModel("torch")->getMesh();
        baseShader = resourceManager->getShader("normalShader").get();
        texture = resourceManager->getTexture("torch.png").get();
        texture2 = resourceManager->getTexture("torch_normal.png").get();

        constexpr float quad[] = {
            -1.0f, -1.0f,
            1.0f, -1.0f,
            -1.0f, 1.0f,
            1.0f, 1.0f,
        };
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quad), quad, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *) 0);
        glBindVertexArray(0);

        // Linie od originu po jednotku na ose + trojúhelníková špička
        std::vector<glm::vec3> gizmoVerts = {
            // X osa (hřídel)
            {0, 0, 0}, {1, 0, 0},
            // Y osa
            {0, 0, 0}, {0, 1, 0},
            // Z osa
            {0, 0, 0}, {0, 0, 1},
        };

        glGenVertexArrays(1, &gizmoVAO);
        glGenBuffers(1, &gizmoVBO);
        glBindVertexArray(gizmoVAO);
        glBindBuffer(GL_ARRAY_BUFFER, gizmoVBO);
        glBufferData(GL_ARRAY_BUFFER, gizmoVerts.size() * sizeof(glm::vec3), gizmoVerts.data(), GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), static_cast<void *>(nullptr));
        glBindVertexArray(0);

        // X
        auto circleX = makeCircleForAxis(glm::vec3(1, 0, 0), 64);
        glGenVertexArrays(1, &circleVAOX);
        glGenBuffers(1, &circleVBOX);
        glBindVertexArray(circleVAOX);
        glBindBuffer(GL_ARRAY_BUFFER, circleVBOX);
        glBufferData(GL_ARRAY_BUFFER, circleX.size() * sizeof(glm::vec3), circleX.data(), GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3,GL_FLOAT,GL_FALSE, sizeof(glm::vec3), (void *) 0);

        // Y
        auto circleY = makeCircleForAxis(glm::vec3(0, 1, 0), 64);
        glGenVertexArrays(1, &circleVAOY);
        glGenBuffers(1, &circleVBOY);
        glBindVertexArray(circleVAOY);
        glBindBuffer(GL_ARRAY_BUFFER, circleVBOY);
        glBufferData(GL_ARRAY_BUFFER, circleY.size() * sizeof(glm::vec3), circleY.data(), GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3,GL_FLOAT,GL_FALSE, sizeof(glm::vec3), (void *) 0);

        // Z
        auto circleZ = makeCircleForAxis(glm::vec3(0, 0, 1), 64);
        glGenVertexArrays(1, &circleVAOZ);
        glGenBuffers(1, &circleVBOZ);
        glBindVertexArray(circleVAOZ);
        glBindBuffer(GL_ARRAY_BUFFER, circleVBOZ);
        glBufferData(GL_ARRAY_BUFFER, circleZ.size() * sizeof(glm::vec3), circleZ.data(), GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3,GL_FLOAT,GL_FALSE, sizeof(glm::vec3), (void *) 0);

        glBindVertexArray(0);
    }

    TorchRenderer::~TorchRenderer() = default;

    void TorchRenderer::render3D(float dt) {
        baseShader->use();
        baseShader->setMat4("view", camera->getViewMatrix());
        baseShader->setMat4("projection", projection);
        baseShader->setInt("diffuseMap", 0);
        baseShader->setInt("normalMap", 1);
        baseShader->setInt("specularMap", 2);
        baseShader->setFloat("alpha", 1.0);
        baseShader->setVec3("viewPos", camera->getPosition());
        baseShader->setBool("parallaxEnable", false);
        baseShader->setBool("fogEnable", fog);
        texture->bind(0);
        texture2->bind(1);
        texture->bind(2);

        glm::vec3 lightPos(cube->getPosition().x, cube->getPosition().y - 6, cube->getPosition().z + 5);
        baseShader->setVec3("lightPos", lightPos);

        texture->bind(0);
        texture2->bind(1);
        texture->bind(2);

        renderScene(baseShader);
    }

    void TorchRenderer::renderShadowMap() {
    }

    void TorchRenderer::beforeRender() {
    }

    void TorchRenderer::afterRender() {
    }

    void TorchRenderer::renderScene(const ShaderManager *shader) {
        glLoadIdentity();
        const glm::mat4 model = cube->getModelMatrix();
        const auto time = static_cast<float>(glfwGetTime());
        const float pulse = 0.5f + 0.5f * sin(time * 4.0f); // osciluje 0..1

        auto meshVertices = mesh->getVertices();

        // 1. Lokální AABB
        glm::vec3 localMin, localMax;
        computeLocalAABB(meshVertices, localMin, localMax);

        // 2. Světový AABB (bere v potaz scale/rotation/translation)
        glm::vec3 worldMin, worldMax;
        computeWorldAABB(model, localMin, localMax, worldMin, worldMax);

        // 3. Center objektu ve světě
        currentWorldCenter = (worldMin + worldMax) * 0.5f;
        currentWorldMin = worldMin;
        currentWorldMax = worldMax;

        // 4. Ring center: pod objektem (Z-up) — malý offset dolů, aby nebyl v konfliktu
        glm::vec3 ringCenter = currentWorldCenter;
        ringCenter.z = worldMin.z - 0.01f;

        // float scaleX = glm::length(glm::vec3(model[0])); // první sloupec
        // float scaleY = glm::length(glm::vec3(model[1]));
        // float scaleZ = glm::length(glm::vec3(model[2]));
        // auto extractScale = glm::vec3(scaleX, scaleY, scaleZ);

        // 5. Radius: vezmeme extenty v X/Y z world AABB
        float extentX = (worldMax.x - worldMin.x) * 0.7f;
        float extentY = (worldMax.y - worldMin.y) * 0.7f;
        float radius = glm::max(extentX, extentY) * 1.18f;

        const auto ringShader = resourceManager->getShader("colorShader");
        // bind shader, nastav uniformy
        ringShader->use();
        ringShader->setVec3("ringCenter", ringCenter);
        ringShader->setMat4("view", camera->getViewMatrix());
        ringShader->setMat4("proj", projection);
        ringShader->setFloat("radius", radius); // např. 1.0f nebo podle velikosti objektu
        ringShader->setFloat("pulse", pulse);
        ringShader->setVec3("color", glm::vec3(1.0f, 1.0f, 0.0f)); // žlutá

        // vykresli billboard
        glBindVertexArray(quadVAO);
        // můžeš zapnout blending pro jemnost
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glDisable(GL_BLEND);
        glBindVertexArray(0);

        glLoadIdentity();
        shader->use();
        shader->setMat4("model", model);

        mesh->bind();
        glDrawElements(GL_TRIANGLES, (int) mesh->getIndices().size(), GL_UNSIGNED_INT, nullptr);

        // glm::vec4 centerClip = projection * camera->getViewMatrix() * glm::vec4(ringCenter, 1.0f);
        // glm::vec3 centerNDC = glm::vec3(centerClip) / centerClip.w;
        //
        // glm::vec3 offsetWorld = ringCenter + glm::vec3(radius, 0.0f, 0.0f);
        // glm::vec4 offsetClip = projection * camera->getViewMatrix() * glm::vec4(offsetWorld, 1.0f);
        // glm::vec3 offsetNDC = glm::vec3(offsetClip) / offsetClip.w;
        //
        // float ndcDiff = fabs(offsetNDC.x - centerNDC.x); // polovina průměru v NDC
        // float desiredNDC = 0.08f; // chtěný minimální poloměr v NDC (nastav podle toho, jak velké chces)
        // if (ndcDiff < desiredNDC) {
        //     // zvětšíme radius tak, aby v NDC bylo aspoň desiredNDC
        //     float scaleFactor = desiredNDC / ndcDiff;
        //     radius *= scaleFactor;
        // }

        drawGizmoAxes(currentWorldCenter, camera->getPosition(), camera->getViewMatrix(), projection, 1080);
        drawRotationGizmo(currentWorldCenter, camera->getPosition(), camera->getViewMatrix(), projection, 1080, 64);
    }

    void TorchRenderer::computeLocalAABB(const std::vector<Vertex> &verts, glm::vec3 &outMin, glm::vec3 &outMax) {
        if (verts.empty()) {
            outMin = outMax = glm::vec3(0.0f);
            return;
        }
        outMin = verts[0].position;
        outMax = verts[0].position;
        for (const auto &v: verts) {
            outMin = glm::min(outMin, v.position);
            outMax = glm::max(outMax, v.position);
        }
    }

    void TorchRenderer::computeWorldAABB(const glm::mat4 &model, const glm::vec3 &localMin, const glm::vec3 &localMax,
                                         glm::vec3 &outWorldMin, glm::vec3 &outWorldMax) {
        glm::vec3 corners[8] = {
            {localMin.x, localMin.y, localMin.z},
            {localMax.x, localMin.y, localMin.z},
            {localMin.x, localMax.y, localMin.z},
            {localMax.x, localMax.y, localMin.z},
            {localMin.x, localMin.y, localMax.z},
            {localMax.x, localMin.y, localMax.z},
            {localMin.x, localMax.y, localMax.z},
            {localMax.x, localMax.y, localMax.z},
        };
        glm::vec3 wMin(std::numeric_limits<float>::infinity());
        glm::vec3 wMax(-std::numeric_limits<float>::infinity());
        for (int i = 0; i < 8; ++i) {
            glm::vec4 transformed = model * glm::vec4(corners[i], 1.0f);
            glm::vec3 p = glm::vec3(transformed);
            wMin = glm::min(wMin, p);
            wMax = glm::max(wMax, p);
        }
        outWorldMin = wMin;
        outWorldMax = wMax;
    }

    // worldCenter = pozice gizma
    // cameraPos je pozice kamery ve světě
    // view, proj jsou matice
    // viewportHeight je výška okna v pixelech
    float TorchRenderer::computeGizmoScale(const glm::vec3 &worldCenter, const glm::vec3 &cameraPos,
                                           float viewportHeight,
                                           float desiredPixelSize /*unused*/ ) {
        glm::vec3 objectSize = (currentWorldMax - currentWorldMin);
        float maxExtent = glm::compMax(objectSize);

        return maxExtent * 1.1f;
    }

    void TorchRenderer::drawGizmoAxes(const glm::vec3 &worldCenter,
                                      const glm::vec3 &cameraPos,
                                      const glm::mat4 &view,
                                      const glm::mat4 &proj,
                                      float viewportHeight) {
        const auto gizmoShader = resourceManager->getShader("gizmoShader");
        if (!gizmoShader) return;

        const float axisLength = computeGizmoScale(worldCenter, cameraPos, viewportHeight, 80.0f);

        gizmoShader->use();
        gizmoShader->setMat4("view", view);
        gizmoShader->setMat4("proj", proj);
        gizmoShader->setFloat("thickness", 3.0f);
        gizmoShader->setVec2("viewportSize", glm::vec2(1920, viewportHeight));
        gizmoShader->setVec3("gizmoCenter", worldCenter);
        gizmoShader->setFloat("scale", 1.0f);

        glBindVertexArray(gizmoVAO);
        glBindBuffer(GL_ARRAY_BUFFER, gizmoVBO);

        // Nastavení vertex atributu pro pozice
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void *) 0);

        std::vector<glm::vec3> vertices;
        vertices.push_back({0, 0, 0});
        vertices.push_back({axisLength, 0, 0});
        vertices.push_back({0, 0, 0});
        vertices.push_back({0, axisLength, 0});
        vertices.push_back({0, 0, 0});
        vertices.push_back({0, 0, axisLength});

        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);

        // Vykreslení čar
        gizmoShader->setBool("isArrowHead", false);
        for (int i = 0; i < 3; i++) {
            gizmoShader->setInt("axisType", i);

            glm::vec3 baseColor = i == 0 ? glm::vec3(1, 0, 0) : (i == 1 ? glm::vec3(0, 1, 0) : glm::vec3(0, 0, 1));

            // Zvýraznění při hoveru
            if (hoveredAxis == Axis::X && i == 0)
                baseColor = glm::vec3(1, 0.6f, 0.6f);

            if (hoveredAxis == Axis::Y && i == 1)
                baseColor = glm::vec3(0.6f, 0.6f, 1);

            if (hoveredAxis == Axis::Z && i == 2)
                baseColor = glm::vec3(0.6f, 0.6f, 1);

            gizmoShader->setVec3("color", baseColor);
            glDrawArrays(GL_LINES, i * 2, 2);
        }

        // Vykreslení šipek
        gizmoShader->setBool("isArrowHead", true);
        for (int i = 0; i < 3; i++) {
            glm::vec3 baseColor = i == 0 ? glm::vec3(1, 0, 0) : (i == 1 ? glm::vec3(0, 1, 0) : glm::vec3(0, 0, 1));
            // Zvýraznění při hoveru
            if ((hoveredAxis == Axis::X || activeAxis == Axis::X) && i == 0)
                baseColor = glm::vec3(1, 0.6f, 0.6f);

            if (hoveredAxis == Axis::Y && i == 1)
                baseColor = glm::vec3(0.6f, 0.6f, 1);

            if (hoveredAxis == Axis::Z && i == 2)
                baseColor = glm::vec3(0.6f, 0.6f, 1);
            gizmoShader->setInt("axisType", i);
            gizmoShader->setVec3("color", baseColor);
            glDrawArrays(GL_LINES, i * 2, 2);
        }

        glBindVertexArray(0);
    }

    void TorchRenderer::drawRotationGizmo(const glm::vec3 &worldCenter,
                                          const glm::vec3 &cameraPos,
                                          const glm::mat4 &view,
                                          const glm::mat4 &proj,
                                          float viewportHeight,
                                          int circleVertexCount) {
        float baseScale = computeGizmoScale(worldCenter, cameraPos, viewportHeight, 80.0f);
        float circleScale = baseScale * 0.8f;

        const auto gizmoShader = resourceManager->getShader("gizmoShader");
        gizmoShader->use();
        gizmoShader->setMat4("view", view);
        gizmoShader->setMat4("proj", proj);
        gizmoShader->setVec3("gizmoCenter", worldCenter);
        gizmoShader->setFloat("scale", circleScale);
        gizmoShader->setFloat("thickness", 2.0f);
        gizmoShader->setVec2("viewportSize", glm::vec2(1080, viewportHeight));
        gizmoShader->setBool("isArrowHead", false);

        // X kruh (gap: přeskočíme pár vrcholů kolem 0 úhlu)
        bool hoverX = (hoveredAxis == Axis::RotateX);
        bool activeX = (activeAxis == Axis::RotateX);
        glm::vec3 baseColorX = activeX
                                   ? glm::vec3(1, 0.3f, 0.3f)
                                   : hoverX
                                         ? glm::vec3(1, 0.6f, 0.6f)
                                         : glm::vec3(1, 0, 0);
        gizmoShader->setVec3("color", baseColorX);
        int gap = circleVertexCount / 16; // velikost gapu
        // vykreslíme dva segmenty: od gap do poloviny a od poloviny+gap do end-gap (dají pocit přerušení)
        glBindVertexArray(circleVAOX);
        glDrawArrays(GL_LINE_STRIP, gap, circleVertexCount / 2 - gap);
        glDrawArrays(GL_LINE_STRIP, circleVertexCount / 2 + gap, circleVertexCount / 2 - gap);

        // Y kruh
        bool hoverY = (hoveredAxis == Axis::RotateY);
        bool activeY = (activeAxis == Axis::RotateY);
        glm::vec3 baseColorY = activeY
                                   ? glm::vec3(0.3f, 1, 0.3f)
                                   : hoverY
                                         ? glm::vec3(0.6f, 1, 0.6f)
                                         : glm::vec3(0, 1, 0);
        gizmoShader->setVec3("color", baseColorY);
        glBindVertexArray(circleVAOY);
        glDrawArrays(GL_LINE_STRIP, gap, circleVertexCount / 2 - gap);
        glDrawArrays(GL_LINE_STRIP, circleVertexCount / 2 + gap, circleVertexCount / 2 - gap);

        // Z kruh
        bool hoverZ = (hoveredAxis == Axis::RotateZ);
        bool activeZ = (activeAxis == Axis::RotateZ);

        glm::vec3 baseColorZ = activeZ
                                   ? glm::vec3(0.3f, 0.3f, 1)
                                   : hoverZ
                                         ? glm::vec3(0.6f, 0.6f, 1)
                                         : glm::vec3(0, 0, 1);
        gizmoShader->setVec3("color", baseColorZ);
        glBindVertexArray(circleVAOZ);
        glDrawArrays(GL_LINE_STRIP, gap, circleVertexCount / 2 - gap);
        glDrawArrays(GL_LINE_STRIP, circleVertexCount / 2 + gap, circleVertexCount / 2 - gap);

        glBindVertexArray(0);
    }

    // vrátí vertexy kruhu v rovině kolmé na axis (normalized axis: e.g. {1,0,0} for X)
    std::vector<glm::vec3> TorchRenderer::makeCircleForAxis(const glm::vec3 &axis, int segments = 64) {
        std::vector<glm::vec3> verts;
        // najdi dvě ortogonální vektory k axis pro parametrizaci kruhu
        glm::vec3 up = glm::abs(axis.y) < 0.99f ? glm::vec3(0, 1, 0) : glm::vec3(1, 0, 0);
        glm::vec3 tangent = glm::normalize(glm::cross(axis, up));
        glm::vec3 bitangent = glm::normalize(glm::cross(axis, tangent));

        for (int i = 0; i <= segments; ++i) {
            float theta = (float) i / (float) segments * glm::two_pi<float>();
            float c = cos(theta);
            float s = sin(theta);
            glm::vec3 point = tangent * c + bitangent * s; // leží v rovině kolmé na axis
            verts.push_back(point); // bude škálováno a posouváno při kreslení
        }
        return verts;
    }

    glm::vec3 TorchRenderer::screenToWorldRay(const glm::vec2 &screenPos, const glm::mat4 &view, const glm::mat4 &proj,
                                              int viewportWidth, int viewportHeight) {
        float x = (2.0f * screenPos.x) / viewportWidth - 1.0f;
        float y = 1.0f - (2.0f * screenPos.y) / viewportHeight;
        glm::vec4 clip = glm::vec4(x, y, -1.0f, 1.0f);
        glm::vec4 eye = glm::inverse(proj) * clip;
        eye.z = -1.0f;
        eye.w = 0.0f;
        glm::vec4 worldDir4 = glm::inverse(view) * eye;
        glm::vec3 worldDir = glm::normalize(glm::vec3(worldDir4));

        return worldDir;
    }

    bool TorchRenderer::closestPointsBetweenLines(const glm::vec3 &p1, const glm::vec3 &d1,
                                                  const glm::vec3 &p2, const glm::vec3 &d2,
                                                  glm::vec3 &outPoint1, glm::vec3 &outPoint2) {
        float a = glm::dot(d1, d1);
        float b = glm::dot(d1, d2);
        float c = glm::dot(d2, d2);
        glm::vec3 r = p1 - p2;
        float d = glm::dot(d1, r);
        float e = glm::dot(d2, r);
        float denom = a * c - b * b;
        if (fabs(denom) < 1e-6f) {
            // téměř rovnoběžné: použij projekci p1 na osu2 jako aproximaci
            outPoint1 = p1;
            float t = glm::dot(d2, p1 - p2) / glm::dot(d2, d2);
            outPoint2 = p2 + d2 * t;
            return false;
        }
        float s = (b * e - c * d) / denom;
        float t = (a * e - b * d) / denom;
        outPoint1 = p1 + d1 * s;
        outPoint2 = p2 + d2 * t;
        return true;
    }

    void TorchRenderer::updateHover(const glm::vec2 &cursor, int width, int height) {
        // glm::vec3 rayDir = screenToWorldRay(cursor, camera->getViewMatrix(), projection, width, height);
        // glm::vec3 rayOrigin = camera->getPosition();
        // float gizmoScale = computeGizmoScale(currentWorldCenter, camera->getPosition(), height);
        // Axis hover = pickTranslateAxis(currentWorldCenter, rayOrigin, rayDir, gizmoScale, cursor, width, height, 12.0f);
        // hoveredAxis = hover;
        glm::mat4 view = camera->getViewMatrix();
        auto rayDir = screenToWorldRay(cursor, view, projection, width, height);
        auto rayOrigin = camera->getPosition();

        float gizmoScale = computeGizmoScale(currentWorldCenter, camera->getPosition(), height, 80.0f);

        // Nejdřív zkusíme translační osy
        Axis translationAxis = pickTranslateAxis(currentWorldCenter, rayOrigin, rayDir,
                                                 gizmoScale, cursor, width, height, 16.0f);

        if (translationAxis != Axis::None) {
            hoveredAxis = translationAxis;
            return;
        }

        // Pokud není vybrána translační osa, zkusíme rotační
        Axis rotationAxis = pickRotationAxis(currentWorldCenter, cursor, width, height);
        hoveredAxis = rotationAxis;
    }

    TorchRenderer::Axis TorchRenderer::pickTranslateAxis(const glm::vec3 &worldCenter,
                                                         const glm::vec3 &rayOrigin,
                                                         const glm::vec3 &rayDir,
                                                         float gizmoScale,
                                                         const glm::vec2 &cursor, // nové: kurzor
                                                         int viewportWidth,
                                                         int viewportHeight,
                                                         float pixelThreshold) {
        struct AxisTest {
            Axis axis;
            glm::vec3 dir;
            float screenDist;
            float worldDist;
            glm::vec3 closestOnAxis;
            glm::vec3 segmentStart;
            glm::vec3 segmentEnd;
        };
        std::vector<AxisTest> tests = {
            {Axis::X, glm::vec3(1, 0, 0), FLT_MAX, FLT_MAX, {}, {}, {}},
            {Axis::Y, glm::vec3(0, 1, 0), FLT_MAX, FLT_MAX, {}, {}, {}},
            {Axis::Z, glm::vec3(0, 0, 1), FLT_MAX, FLT_MAX, {}, {}, {}}
        };

        float worldThreshold = gizmoScale * 0.25f; // trochu větší tolerance

        for (auto &test: tests) {
            test.segmentStart = worldCenter;
            test.segmentEnd = worldCenter + test.dir * gizmoScale;

            glm::vec3 pointOnRay, pointOnAxisInfinite;
            closestPointsBetweenLines(rayOrigin, rayDir, test.segmentStart, test.dir, pointOnRay, pointOnAxisInfinite);

            glm::vec3 toAxis = pointOnAxisInfinite - test.segmentStart;
            float projLen = glm::dot(toAxis, test.dir);
            projLen = glm::clamp(projLen, 0.0f, gizmoScale);
            glm::vec3 pointOnAxisSegment = test.segmentStart + test.dir * projLen;
            test.closestOnAxis = pointOnAxisSegment;

            test.worldDist = glm::length(pointOnRay - pointOnAxisSegment);

            glm::vec2 screenA = worldToScreen(test.segmentStart, camera->getViewMatrix(), projection, viewportWidth,
                                              viewportHeight);
            glm::vec2 screenB = worldToScreen(test.segmentEnd, camera->getViewMatrix(), projection, viewportWidth,
                                              viewportHeight);
            test.screenDist = pointToSegmentDistance2D(cursor, screenA, screenB);
        }

        // primární filtr: musí být blízko v screen-space i world-space
        Axis bestAxis = Axis::None;
        float bestScreen = FLT_MAX;
        for (auto &t: tests) {
            if (t.screenDist < pixelThreshold && t.worldDist < worldThreshold) {
                if (t.screenDist < bestScreen) {
                    bestScreen = t.screenDist;
                    bestAxis = t.axis;
                }
            }
        }
        if (bestAxis != Axis::None) {
            return bestAxis;
        }

        // fallback: kombinované skóre (normalizované)
        struct ScoreEntry {
            Axis axis;
            float score;
        };
        std::vector<ScoreEntry> scoreList;
        for (auto &t: tests) {
            float normScreen = t.screenDist / pixelThreshold;
            float normWorld = t.worldDist / worldThreshold;
            float combined = normScreen * normScreen + normWorld * normWorld;
            scoreList.push_back({t.axis, combined});
        }
        std::sort(scoreList.begin(), scoreList.end(), [](const ScoreEntry &a, const ScoreEntry &b) {
            return a.score < b.score;
        });
        if (!scoreList.empty() && scoreList[0].score < 1.0f) {
            return scoreList[0].axis;
        }

        // poslední fallback: nejmenší screenDist, i když worldDist bude větší
        auto bestScreenIt = std::min_element(tests.begin(), tests.end(),
                                             [](const AxisTest &a, const AxisTest &b) {
                                                 return a.screenDist < b.screenDist;
                                             });
        if (bestScreenIt != tests.end() && bestScreenIt->screenDist < pixelThreshold * 2.0f) {
            return bestScreenIt->axis;
        }

        return Axis::None;
    }

    void TorchRenderer::onMouseDown(const glm::vec2 &cursor, int width, int height) {
        glm::vec3 rayDir = screenToWorldRay(cursor, camera->getViewMatrix(), projection, width, height);
        glm::vec3 rayOrigin = camera->getPosition();

        float gizmoScale = computeGizmoScale(currentWorldCenter, camera->getPosition(), height, 80.0f);

        Axis picked = pickTranslateAxis(currentWorldCenter, rayOrigin, rayDir, gizmoScale, cursor, width, height,
                                        16.0f);
        Axis pickedRotate = pickRotationAxis(currentWorldCenter, cursor, width, height);
        if (picked == Axis::None && pickedRotate == Axis::None) {
            return;
        }

        if (hoveredAxis == Axis::X || hoveredAxis == Axis::Y ||
            hoveredAxis == Axis::Z) {
            mode = InteractionMode::Translating;
            activeAxis = picked;
            axisDir = (picked == Axis::X)
                          ? glm::vec3(1, 0, 0)
                          : (picked == Axis::Y)
                                ? glm::vec3(0, 1, 0)
                                : glm::vec3(0, 0, 1);
            axisDir = glm::normalize(axisDir);

            // referenční začátek osy: pozice objektu při startu
            dragLineOrigin = cube->getPosition();
            startObjectPos = cube->getPosition();

            // spočítat nejbližší bod na ose (closestOnAxis) k paprsku -> to je startGrabPoint
            glm::vec3 pointOnRay, closestOnAxis;
            bool valid = closestPointsBetweenLines(rayOrigin, rayDir, dragLineOrigin, axisDir, pointOnRay,
                                                   closestOnAxis);
            if (!valid) {
                // fallback: projekce rayOrigin na osu
                glm::vec3 toOrigin = rayOrigin - dragLineOrigin;
                float proj = glm::dot(toOrigin, axisDir);
                closestOnAxis = dragLineOrigin + axisDir * proj;
            }

            startGrabPoint = closestOnAxis;
            // parametr podél osy od dragLineOrigin k místu uchopení
            startAxisParam = glm::dot(startGrabPoint - dragLineOrigin, axisDir);
        } else if (hoveredAxis == Axis::RotateX || hoveredAxis == Axis::RotateY ||
                   hoveredAxis == Axis::RotateZ) {
            mode = InteractionMode::Rotating;
            activeAxis = pickedRotate;

            originalRotation[0] = cube->getRotationX();
            originalRotation[1] = cube->getRotationY();
            originalRotation[2] = cube->getRotationZ();

            // Výpočet počátečního úhlu pro rotaci
            glm::vec2 centerScreen = worldToScreen(currentWorldCenter,
                                                   camera->getViewMatrix(),
                                                   projection, width, height);
            glm::vec2 toMouse = cursor - centerScreen;
            rotationStartAngle = atan2(toMouse.y, toMouse.x);
        }


        // std::cout << "startGrabPoint: " << vec3_to_string(startGrabPoint)
        //         << " startObjectPos: " << vec3_to_string(startObjectPos)
        //         << " startAxisParam: " << startAxisParam << std::endl;
    }

    // pomocná: promítne světový bod do screen space (pixelů)
    glm::vec3 TorchRenderer::worldToScreen(const glm::vec3 &worldPos,
                                           const glm::mat4 &view,
                                           const glm::mat4 &proj,
                                           int width,
                                           int height) {
        glm::vec4 clipSpace = projection * view * glm::vec4(worldPos, 1.0f);
        glm::vec3 ndc = glm::vec3(clipSpace) / clipSpace.w;
        glm::vec2 screenPos = glm::vec2(
            (ndc.x + 1.0f) * 0.5f * width,
            (1.0f - ndc.y) * 0.5f * height
        );

        return glm::vec3(screenPos, ndc.z);
    }

    void TorchRenderer::onMouseMove(const glm::vec2 &cursor, int width, int height) {
        updateHover(cursor, width, height);
        if (activeAxis == Axis::None) {
            return;
        }

        if (mode == InteractionMode::Translating) {
            glm::vec3 rayDir = screenToWorldRay(cursor, camera->getViewMatrix(), projection, width, height);
            glm::vec3 rayOrigin = camera->getPosition();

            // najdi aktuální nejbližší bod na nekonečné ose (procházející dragLineOrigin)
            glm::vec3 pointOnRay, closestOnAxis;
            bool valid = closestPointsBetweenLines(rayOrigin, rayDir, dragLineOrigin, axisDir, pointOnRay,
                                                   closestOnAxis);
            if (!valid) {
                // fallback: projekce rayOrigin na osu
                glm::vec3 toOrigin = rayOrigin - dragLineOrigin;
                float proj = glm::dot(toOrigin, axisDir);
                closestOnAxis = dragLineOrigin + axisDir * proj;
            }

            // aktuální parametr podél osy
            float currentParam = glm::dot(closestOnAxis - dragLineOrigin, axisDir);

            // Korekce pro citlivost osy podle délky projekce na obrazovku
            glm::vec3 axisScreenA = worldToScreen(dragLineOrigin, camera->getViewMatrix(), projection, width, height);
            glm::vec3 axisScreenB = worldToScreen(dragLineOrigin + axisDir, camera->getViewMatrix(), projection, width, height);
            float axisScreenLength = glm::length(axisScreenB - axisScreenA);

            // Základní délka pro srovnání (např. osa X)
            float referenceScreenLength = 100.0f; // experimentuj s hodnotou
            float scale = (axisScreenLength > 0.0001f) ? (referenceScreenLength / axisScreenLength) : 1.0f;
            float deltaParam = (currentParam - startAxisParam) * scale;

            // float deltaParam = currentParam - startAxisParam;

            // nová pozice objektu: startovní + posun podél osy
            glm::vec3 newPos = startObjectPos + axisDir * deltaParam;
            cube->setPosition(newPos);
            currentWorldCenter = newPos;
        } else if (mode == InteractionMode::Rotating) {
            glm::vec2 centerScreen = worldToScreen(currentWorldCenter,
                                                   camera->getViewMatrix(),
                                                   projection, width, height);
            glm::vec2 toMouse = cursor - centerScreen;
            float currentAngle = atan2(toMouse.y, toMouse.x);
            float deltaAngle = currentAngle - rotationStartAngle;

            // Kopírujeme původní rotace
            float newRotateX = originalRotation[0];
            float newRotateY = originalRotation[1];
            float newRotateZ = originalRotation[2];

            // Upravíme příslušnou rotační komponentu
            switch (activeAxis) {
                case Axis::RotateX:
                    newRotateX += glm::degrees(deltaAngle);
                    break;
                case Axis::RotateY:
                    newRotateY += glm::degrees(deltaAngle);
                    break;
                case Axis::RotateZ:
                    newRotateZ += glm::degrees(deltaAngle);
                    break;
                default:
                    break;
            }

            // Aplikujeme nové rotace
            cube->setRotationX(newRotateX);
            cube->setRotationY(newRotateY);
            cube->setRotationZ(newRotateZ);
        }

        // debug výpisy
        // std::cout << "closestOnAxis: " << vec3_to_string(closestOnAxis)
        //         << " deltaParam: " << deltaParam
        //         << " newPos: " << vec3_to_string(newPos) << std::endl;
    }

    void TorchRenderer::onMouseUp() {
        mode = InteractionMode::None;
        activeAxis = Axis::None;
    }

    float TorchRenderer::pointToSegmentDistance2D(const glm::vec2 &p, const glm::vec2 &a, const glm::vec2 &b) {
        glm::vec2 ab = b - a;
        float t = glm::dot(p - a, ab) / glm::dot(ab, ab);
        t = glm::clamp(t, 0.0f, 1.0f);
        glm::vec2 projection = a + ab * t;
        return glm::length(p - projection);
    }


    TorchRenderer::Axis TorchRenderer::pickRotationAxis(const glm::vec3 &worldCenter,
                                                        const glm::vec2 &cursor,
                                                        int viewportWidth,
                                                        int viewportHeight,
                                                        float pixelThreshold) {
        glm::mat4 view = camera->getViewMatrix();
        float gizmoScale = computeGizmoScale(worldCenter, camera->getPosition(), viewportHeight, 80.0f);

        // Vytvořit projekce kružnic pro každou osu
        std::vector<glm::vec2> screenPointsX;
        std::vector<glm::vec2> screenPointsY;
        std::vector<glm::vec2> screenPointsZ;

        // Získat body kružnic
        auto circleX = makeCircleForAxis(glm::vec3(1, 0, 0), 32);
        auto circleY = makeCircleForAxis(glm::vec3(0, 1, 0), 32);
        auto circleZ = makeCircleForAxis(glm::vec3(0, 0, 1), 32);

        // Převést body do screen space
        for (const auto &point: circleX) {
            glm::vec3 worldPoint = worldCenter + point * gizmoScale;
            screenPointsX.push_back(worldToScreen(worldPoint, view, projection, viewportWidth, viewportHeight));
        }
        for (const auto &point: circleY) {
            glm::vec3 worldPoint = worldCenter + point * gizmoScale;
            screenPointsY.push_back(worldToScreen(worldPoint, view, projection, viewportWidth, viewportHeight));
        }
        for (const auto &point: circleZ) {
            glm::vec3 worldPoint = worldCenter + point * gizmoScale;
            screenPointsZ.push_back(worldToScreen(worldPoint, view, projection, viewportWidth, viewportHeight));
        }

        // Najít nejbližší vzdálenost pro každou osu
        float minDistX = std::numeric_limits<float>::max();
        float minDistY = std::numeric_limits<float>::max();
        float minDistZ = std::numeric_limits<float>::max();

        // Kontrola vzdálenosti pro každou kružnici
        for (size_t i = 0; i < screenPointsX.size(); i++) {
            size_t next = (i + 1) % screenPointsX.size();
            minDistX = std::min(minDistX,
                                pointToSegmentDistance2D(cursor, screenPointsX[i], screenPointsX[next]));
            minDistY = std::min(minDistY,
                                pointToSegmentDistance2D(cursor, screenPointsY[i], screenPointsY[next]));
            minDistZ = std::min(minDistZ,
                                pointToSegmentDistance2D(cursor, screenPointsZ[i], screenPointsZ[next]));
        }

        // Najít nejbližší osu
        float minDist = std::min({minDistX, minDistY, minDistZ});

        if (minDist > pixelThreshold) {
            return Axis::None;
        }

        if (minDist == minDistX) return Axis::RotateX;
        if (minDist == minDistY) return Axis::RotateY;
        return Axis::RotateZ;
    }
} // Renderer
