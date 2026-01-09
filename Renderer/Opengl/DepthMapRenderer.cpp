#include "DepthMapRenderer.h"

namespace Renderer {
    DepthMapRenderer::DepthMapRenderer(Camera *camera, const glm::mat4 &proj, ResourceManager *resManager) {
        resourceManager = resManager;
        this->camera = camera;
        this->projection = proj;
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
                         SHADOW_WIDTH, SHADOW_HEIGHT, 3, 0,
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
        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
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
        // TODO: toto je tu jen proto, ze to nastavi lightPos, ale jeste vice dulezite lightSpaceMatrix
        shader->use();

        int index = 0;
        constexpr int NUM_CASCADES = 3;
        float cascadeEnds[NUM_CASCADES];

        float lambda = 0.95f;
        float nearClip = 0.1f;
        float farClip = 1000.0f;

        for (int i = 0; i < NUM_CASCADES; i++) {
            float p = static_cast<float>(i + 1) / static_cast<float>(NUM_CASCADES);
            float logSplit = nearClip * std::pow(farClip / nearClip, p);
            float linSplit = nearClip + (farClip - nearClip) * p;
            cascadeEnds[i] = lambda * logSplit + (1.0f - lambda) * linSplit;
        }

        shader->setFloat("cascadeEnds" + std::to_string(0), cascadeEnds[0]);
        shader->setFloat("cascadeEnds" + std::to_string(1), cascadeEnds[1]);
        shader->setFloat("cascadeEnds" + std::to_string(2), cascadeEnds[2]);

        const auto basicShader = resourceManager->getShader("basicShader");
        basicShader->use();
        index = 0;
        for (const auto &lightSpaceMatrice: lightSpaceMatrices) {
            basicShader->setMat4("lightSpaceMatrix" + std::to_string(index), lightSpaceMatrice);
            index++;
        }

        basicShader->setFloat("cascadeEnds" + std::to_string(0), cascadeEnds[0]);
        basicShader->setFloat("cascadeEnds" + std::to_string(1), cascadeEnds[1]);
        basicShader->setFloat("cascadeEnds" + std::to_string(2), cascadeEnds[2]);
    }

    void DepthMapRenderer::bind(const int index, const glm::mat4 &lightSpaceMatrix) const {
        shader->use();
        shader->setMat4("lightSpaceMatrix" + std::to_string(index), lightSpaceMatrix);
        const auto simpleDepthShader = resourceManager->getShader("shadowDepthShader");
        simpleDepthShader->use();
        simpleDepthShader->setMat4("lightSpaceMatrix", lightSpaceMatrix);
    }

    std::vector<glm::mat4> DepthMapRenderer::computeLightSpaceMatrix(shared_ptr<DirectionalLight> &light) {
        lightSpaceMatrices.clear();
        lightSpaceMatrices.resize(NUM_CASCADES);

        glm::vec3 lightDir = glm::normalize(light->getDirection());

        for (int i = 0; i < NUM_CASCADES; ++i) {
            float cameraFar = 80.0f;
            float cameraNear = 0.1f;

            float cascadeNear = (i == 0) ? cameraNear : cameraNear + cascadeSplits[i - 1] * (cameraFar - cameraNear);
            float cascadeFar = cameraNear + cascadeSplits[i] * (cameraFar - cameraNear);

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

            float zMargin = 50.0f;
            minZ -= zMargin;
            maxZ += zMargin;

            glm::mat4 lightProjection = glm::ortho(minX, maxX, minY, maxY, -maxZ, -minZ);

            lightSpaceMatrices[i] = lightProjection * lightView;
        }

        return lightSpaceMatrices;
    }

    std::vector<glm::vec3> DepthMapRenderer::getFrustumCornersWorldSpace(
        float nearPlane,
        float farPlane
    ) const {
        const float aspect = static_cast<float>(1920) / 1080;
        float fov = glm::radians(camera->getZoom());
        float tanHalfFov = tanf(fov * 0.5f);

        std::vector<glm::vec3> corners(8);

        float nh = nearPlane * tanHalfFov;
        float nw = nh * aspect;
        float fh = farPlane * tanHalfFov;
        float fw = fh * aspect;

        glm::vec3 nc = camera->getPosition() + camera->getFront() * nearPlane;
        glm::vec3 fc = camera->getPosition() + camera->getFront() * farPlane;
        glm::vec3 camUp = camera->getUp();
        glm::vec3 camRight = camera->getRight();

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
