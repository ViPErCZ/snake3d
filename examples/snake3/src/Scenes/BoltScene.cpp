#include "BoltScene.h"

#include <random>

#include "Renderer/Opengl/Model/Standard/2D/QuadNode2D.h"

namespace Scenes {
    namespace {
        mt19937 &rng() {
            static mt19937 gen(random_device{}());
            return gen;
        }

        float randf(const float lo, const float hi) {
            return uniform_real_distribution(lo, hi)(rng());
        }
    }

    BoltScene::BoltScene(
        const shared_ptr<DirectionalLight> &directionalLight,
        const vector<shared_ptr<SpotLight>> &spotLights,
        const vector<shared_ptr<PointLight>> &pointLights,
        const shared_ptr<RenderManager> &rendererManager,
        const shared_ptr<Camera> &camera, const glm::mat4 &projection,
        const shared_ptr<ResourceManager> &rm, const int width, const int height)
        : Scene(directionalLight, spotLights, pointLights, rendererManager, camera, projection, rm, width, height) {
    }

    void BoltScene::init(const int priority) {
        Scene::init(priority);

        const auto boltShader = resourceManager->getShader("boltShader");
        boltLines = make_shared<BoltLinesNode2D>(projection, boltShader);
        boltMeshNode = make_shared<MeshNode2D>(contextState, boltLines, resourceManager);
        addMeshNode2D(boltMeshNode, 90);

        const auto flashShader = resourceManager->getShader("basic2d");
        const auto flashQuad = make_shared<QuadNode2D>(10000.0f, 10000.0f);
        flashQuad->setBlending(Blending::Translucent);
        flashQuad->setDepthTest(false);
        flashQuad->setDepthWrite(false);

        flashMaterial = make_shared<ShaderMaterial>(flashShader);
        flashMaterial->setUniform("color", glm::vec3(1.0f, 1.0f, 1.0f));
        flashMaterial->setUniform("alpha", 0.0f);
        flashMaterial->setUniform("useMaterial", true);
        flashQuad->setMaterial(flashMaterial);

        flashNode = make_shared<MeshNode2D>(contextState, flashQuad, resourceManager);
        flashNode->setPosition({static_cast<float>(width) * 0.5f, static_cast<float>(height) * 0.5f, 0.0f});
        addMeshNode2D(flashNode, 80);

        nextBoltIn = randomNextBoltTime();
        boltTimer.start();
    }

    void BoltScene::update() {
        Scene::update();

        boltTimer.update();
        const float dt = static_cast<float>(boltTimer.getDeltaTime());

        if (boltActive) {
            boltElapsed += dt;
            if (boltElapsed >= kBoltDuration) {
                boltActive = false;
                boltLines->clearSegments();
            }
        }

        if (flashAlpha > 0.0f) {
            flashAlpha -= kFlashDecay * dt;
            if (flashAlpha < 0.0f) flashAlpha = 0.0f;
            flashMaterial->setUniform("alpha", flashAlpha);
        }

        if (static_cast<float>(boltTimer.getElapsedTime()) >= nextBoltIn) {
            triggerBolt();
        }
    }

    void BoltScene::triggerBolt() {
        boltTimer.reset();
        boltTimer.update();
        nextBoltIn = randomNextBoltTime();
        boltActive = true;
        boltElapsed = 0.0f;

        const glm::vec3 camPos = camera->getPosition();
        const glm::vec3 camFront = camera->getFront();
        const glm::vec3 camRight = glm::normalize(glm::cross(camFront, glm::vec3(0.0f, 1.0f, 0.0f)));

        const float forwardOff = randf(10.0f, 25.0f);
        const float sideOff = randf(-8.0f, 8.0f);
        const float heightOff = randf(15.0f, 25.0f);

        const glm::vec3 start = camPos + camFront * forwardOff + camRight * sideOff + glm::vec3(0.0f, heightOff, 0.0f);
        const glm::vec3 end = start + glm::vec3(randf(-2.0f, 2.0f), -heightOff - 5.0f, randf(-2.0f, 2.0f));

        generateSegments(start, end);

        flashAlpha = 0.9f;
        flashMaterial->setUniform("alpha", flashAlpha);
    }

    void BoltScene::generateSegments(const glm::vec3 &start, const glm::vec3 &end) const {
        constexpr int numSegments = 25;

        const glm::vec3 direction = end - start;
        const glm::vec3 step = direction / static_cast<float>(numSegments);
        const glm::vec3 perp = glm::normalize(glm::cross(direction, glm::vec3(0.0f, 1.0f, 0.0f)));

        vector<BoltLinesNode2D::Segment> segs;
        segs.reserve(numSegments + 8);

        glm::vec3 current = start;
        for (int i = 0; i < numSegments; i++) {
            constexpr float maxOffset = 1.5f;
            glm::vec3 next = current + step;
            if (i != numSegments - 1) {
                next += perp * randf(-maxOffset, maxOffset);
                next += glm::vec3(0.0f, 1.0f, 0.0f) * randf(-maxOffset * 0.5f, maxOffset * 0.5f);
            }

            const float t = static_cast<float>(i) / numSegments;
            segs.push_back({current, next, randf(0.8f, 1.2f) * (1.0f - t * 0.3f)});

            if (i > 0 && i < numSegments - 2 && randf(0.0f, 1.0f) < 0.2f) {
                const glm::vec3 branchEnd = current + step * 0.5f + perp * randf(-maxOffset * 2.0f, maxOffset * 2.0f);
                segs.push_back({current, branchEnd, randf(0.4f, 0.7f)});
            }

            current = next;
        }

        if (!segs.empty()) segs.back().end = end;
        boltLines->setSegments(segs);
    }

    float BoltScene::randomNextBoltTime() {
        return randf(5.0f, 15.0f);
    }
} // Scenes
