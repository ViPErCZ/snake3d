#include <snake3d/Renderer/Opengl/DepthMapRenderer.h>

#include <limits>

using namespace Manager;
using namespace Lights;
using std::shared_ptr;

namespace Renderer {
    DepthMapRenderer::DepthMapRenderer(Camera *camera, const glm::mat4 &proj, ResourceManager *resManager,
                                       const int shadowResolution) {
        resourceManager = resManager;
        this->camera = camera;
        this->projection = proj;
        shadowRes = shadowResolution > 0 ? shadowResolution : SHADOW_WIDTH;
        shader = resourceManager->getShader("shadowShader").get();
        shader->use();
        shader->setMat4("projection", projection);
        shader->setInt("diffuseMap", 0);
        shader->setInt("shadowMap", 4);
        shader->setInt("normalMap", 2);
        shader->setInt("specularMap", 3);
        shader->setFloat("alpha", 1.0);

        glGenFramebuffers(1, &depthMapFBO);
        glGenTextures(1, &depthMap); {
            glBindTexture(GL_TEXTURE_2D_ARRAY, depthMap);
            glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_DEPTH_COMPONENT,
                         shadowRes, shadowRes, 3, 0,
                         GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
            glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

            constexpr float borderColor[] = {1.0, 1.0, 1.0, 1.0};
            glTexParameterfv(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_BORDER_COLOR, borderColor);
            glBindTexture(GL_TEXTURE_2D_ARRAY, 0);

            const auto texture = std::make_shared<TextureManager>();

            texture->addTexture(depthMap);
            resourceManager->replaceTexture("depth", texture);
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void DepthMapRenderer::beforeRender(const int index) const {
        glDisable(GL_BLEND);
        glViewport(0, 0, shadowRes, shadowRes);
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
        glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                                  depthMap, 0, index);
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            std::cerr << "Framebuffer not complete!" << std::endl;
        }
        glClear(GL_DEPTH_BUFFER_BIT);
    }

    void DepthMapRenderer::render(float dt) const {
        shader->use();

        shader->setFloat("cascadeEnds0", cascadeEndsWorld[0]);
        shader->setFloat("cascadeEnds1", cascadeEndsWorld[1]);
        shader->setFloat("cascadeEnds2", cascadeEndsWorld[2]);

        const glm::vec3 shadowCenter = camera->getStickyPoint()
            ? glm::vec3(camera->getStickyPoint()->getModelMatrix() * glm::vec4(0, 0, 0, 1))
            : camera->getPosition();

        // Shadow-map uniforms must reach every 3D program that reads them.
        // After B fáze each material permutation (basicShader|features,
        // basicShader|features|HoleMap, basicShader|features|Bones, ...) is
        // a distinct GL program; writing only to ResourceManager's
        // "basicShader" entry left plane / snake-tile / props with stale
        // matrices, which is why plane shadows disappeared after the hole
        // map split. Iterating the registry's cache broadcasts to all of
        // them. setMat4 / setFloat / setVec3 on a uniform that doesn't
        // exist in a given program is a silent no-op (location -1), so
        // particle / 2D / skybox shaders also pass through harmlessly.
        const auto registry = resourceManager->getShaderRegistry();
        if (!registry) {
            return;
        }
        for (const auto& [key, program] : registry->cachedPrograms()) {
            program->use();
            for (int i = 0; i < static_cast<int>(lightSpaceMatrices.size()); ++i) {
                program->setMat4("lightSpaceMatrix" + std::to_string(i), lightSpaceMatrices[i]);
            }
            program->setFloat("cascadeEnds0", cascadeEndsWorld[0]);
            program->setFloat("cascadeEnds1", cascadeEndsWorld[1]);
            program->setFloat("cascadeEnds2", cascadeEndsWorld[2]);
            program->setVec3("shadowCenter", shadowCenter);
        }
    }

    void DepthMapRenderer::bind(const int index, const glm::mat4 &lightSpaceMatrix) const {
        shader->use();
        shader->setMat4("lightSpaceMatrix" + std::to_string(index), lightSpaceMatrix);
        const auto simpleDepthShader = resourceManager->getShader("shadowDepthShader");
        simpleDepthShader->use();
        simpleDepthShader->setMat4("lightSpaceMatrix", lightSpaceMatrix);
    }

    std::vector<glm::mat4> DepthMapRenderer::computeLightSpaceMatrix(const shared_ptr<DirectionalLight> &light,
                                                                      const glm::vec3 /*sceneMin*/, const glm::vec3 /*sceneMax*/) {
        lightSpaceMatrices.clear();
        lightSpaceMatrices.resize(NUM_CASCADES);

        const glm::vec3 lightDir = glm::normalize(light->getDirection());

        // --- RTS-style stable focus-box shadow (opt-in) -------------------------
        if (focusBoxMode) {
            // Ground focus point: where the camera's view ray meets y = 0.
            const glm::vec3 camPos = camera->getPosition();
            const glm::vec3 camFront = camera->getFront();
            const float t = (std::fabs(camFront.y) > 1e-4f) ? (-camPos.y / camFront.y) : 0.0f;
            glm::vec3 focus = (t > 0.0f) ? camPos + camFront * t : camPos;
            focus.y = 0.0f;

            // Box scales with camera height so it always covers the visible area, but
            // stays tight enough for crisp, stable shadows. baseRadius is the half-extent
            // at the default camera height (~30); grow it with how high we are.
            const float R = std::max(focusBoxRadius, focusBoxRadius * (camPos.y / 30.0f));

            const glm::mat4 lightView = glm::lookAt(focus + lightDir, focus, glm::vec3(0, 1, 0));

            // Tight AABB on the ground (±R in XZ) extended in Y to hold the casters.
            float minX = std::numeric_limits<float>::max(), maxX = std::numeric_limits<float>::lowest();
            float minY = std::numeric_limits<float>::max(), maxY = std::numeric_limits<float>::lowest();
            float minZ = std::numeric_limits<float>::max(), maxZ = std::numeric_limits<float>::lowest();
            for (float dx = -R; dx <= R; dx += 2.0f * R)
                for (float dz = -R; dz <= R; dz += 2.0f * R)
                    for (float wy = -2.0f; wy <= 18.0f; wy += 20.0f) {
                        const glm::vec4 trf = lightView * glm::vec4(focus.x + dx, wy, focus.z + dz, 1.0f);
                        minX = std::min(minX, trf.x); maxX = std::max(maxX, trf.x);
                        minY = std::min(minY, trf.y); maxY = std::max(maxY, trf.y);
                        minZ = std::min(minZ, trf.z); maxZ = std::max(maxZ, trf.z);
                    }
            constexpr float zPad = 5.0f;
            glm::mat4 lightProjection = glm::ortho(minX, maxX, minY, maxY, -maxZ - zPad, -minZ + zPad);

            // Texel-snap to kill shimmer while panning.
            glm::mat4 shadowMatrix = lightProjection * lightView;
            glm::vec4 shadowOrigin = shadowMatrix * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
            shadowOrigin *= static_cast<float>(shadowRes) / 2.0f;
            glm::vec4 roundOffset = (glm::round(shadowOrigin) - shadowOrigin) * (2.0f / static_cast<float>(shadowRes));
            roundOffset.z = 0.0f; roundOffset.w = 0.0f;
            lightProjection[3] += roundOffset;

            // The focus box is a SINGLE tight ortho box, so all cascades would be
            // identical and every fragment samples cascade 0 (cascadeEnds0 = 1e9).
            // Rendering the other cascades was pure waste - 3x the shadow draws+fill
            // for layers never read. Emit ONE cascade: the depth pass renders the
            // scene into layer 0 once, the sampling shader routes everything to it.
            const glm::mat4 m = lightProjection * lightView;
            lightSpaceMatrices.assign(1, m);
            cascadeEndsWorld[0] = 1e9f;          // every fragment lands in cascade 0
            cascadeEndsWorld[1] = 1e9f;
            cascadeEndsWorld[2] = 1e9f;
            return lightSpaceMatrices;
        }

        constexpr float cameraFar = 80.0f;
        constexpr float cameraNear = 0.1f;

        for (int i = 0; i < NUM_CASCADES; ++i) {
            const float cascadeNear = (i == 0) ? cameraNear : cameraNear + cascadeSplits[i - 1] * (cameraFar - cameraNear);
            const float cascadeFar  = cameraNear + cascadeSplits[i] * (cameraFar - cameraNear);
            cascadeEndsWorld[i] = cascadeFar;

            auto frustumCorners = getFrustumCornersWorldSpace(cascadeNear, cascadeFar);

            glm::vec3 center(0.0f);
            for (auto &corner: frustumCorners) center += corner;
            center /= static_cast<float>(frustumCorners.size());

            glm::mat4 lightView = glm::lookAt(
                center + lightDir,
                center,
                glm::vec3(0, 1, 0)
            );

            float minX = std::numeric_limits<float>::max();
            float maxX = std::numeric_limits<float>::lowest();
            float minY = std::numeric_limits<float>::max();
            float maxY = std::numeric_limits<float>::lowest();
            float minZ = std::numeric_limits<float>::max();
            float maxZ = std::numeric_limits<float>::lowest();

            for (auto &corner: frustumCorners) {
                glm::vec4 trf = lightView * glm::vec4(corner, 1.0f);
                minX = std::min(minX, trf.x);
                maxX = std::max(maxX, trf.x);
                minY = std::min(minY, trf.y);
                maxY = std::max(maxY, trf.y);
                minZ = std::min(minZ, trf.z);
                maxZ = std::max(maxZ, trf.z);
            }

            constexpr float zMargin = 50.0f;
            minZ -= zMargin;
            maxZ += zMargin;

            glm::mat4 lightProjection = glm::ortho(minX, maxX, minY, maxY, -maxZ, -minZ);

            // Snap world origin to texel grid to eliminate shadow shimmer on camera movement
            glm::mat4 shadowMatrix = lightProjection * lightView;
            glm::vec4 shadowOrigin = shadowMatrix * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
            shadowOrigin *= static_cast<float>(shadowRes) / 2.0f;
            glm::vec4 roundOffset = (glm::round(shadowOrigin) - shadowOrigin) * (2.0f / static_cast<float>(shadowRes));
            roundOffset.z = 0.0f;
            roundOffset.w = 0.0f;
            lightProjection[3] += roundOffset;

            lightSpaceMatrices[i] = lightProjection * lightView;
        }

        return lightSpaceMatrices;
    }

    std::vector<glm::vec3> DepthMapRenderer::getFrustumCornersWorldSpace(
        const float nearPlane,
        const float farPlane
    ) const {
        constexpr float aspect = static_cast<float>(1920) / 1080;
        const float fov = glm::radians(camera->getZoom());
        const float tanHalfFov = tanf(fov * 0.5f);

        std::vector<glm::vec3> corners(8);

        const float nh = nearPlane * tanHalfFov;
        const float nw = nh * aspect;
        const float fh = farPlane * tanHalfFov;
        const float fw = fh * aspect;

        const glm::vec3 camFront = camera->getFront();
        const glm::vec3 nc = camera->getPosition() + camFront * nearPlane;
        const glm::vec3 fc = camera->getPosition() + camFront * farPlane;
        // Derive an ORTHONORMAL right/up from front + up instead of trusting the
        // camera's cached right/up members. Cameras driven by setFront/setUp (e.g.
        // the RTS top-down cam) never run updateCameraVectors, so getRight() is stale
        // and getUp() isn't perpendicular to front -> the frustum corners come out
        // skewed and the cascade ortho fits a garbage volume (shadows stretch across
        // the map, especially while panning). For a camera whose right is already
        // consistent (cross(front, up)) this is a no-op.
        const glm::vec3 camRight = glm::normalize(glm::cross(camFront, camera->getUp()));
        const glm::vec3 camUp = glm::normalize(glm::cross(camRight, camFront));

        // near plane
        corners[0] = nc + camUp * nh - camRight * nw;
        corners[1] = nc + camUp * nh + camRight * nw;
        corners[2] = nc - camUp * nh + camRight * nw;
        corners[3] = nc - camUp * nh - camRight * nw;

        // far plane
        corners[4] = fc + camUp * fh - camRight * fw;
        corners[5] = fc + camUp * fh + camRight * fw;
        corners[6] = fc - camUp * fh + camRight * fw;
        corners[7] = fc - camUp * fh - camRight * fw;

        return corners;
    }
} // Renderer
