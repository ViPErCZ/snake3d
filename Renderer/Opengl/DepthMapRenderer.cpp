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
        glGenTextures(1, &depthMap);

        {
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
            resourceManager->addTexture("depth", texture);
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        lightPos = {0.0f, 7.0f, 11.0f};

        // glm::mat4 lightProjection, lightView;
        // float near_plane = 1.0f, far_plane = 1.17549e-38f;
        // //lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
        // lightProjection = glm::ortho(-1.5f, 4.5f, -1.5f, 4.5f, 1.0f, 15.0f);
        // lightView = glm::lookAt(lightPos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0, 1.0, 0.0));
        // lightSpaceMatrix = lightProjection * lightView;
        // const auto simpleDepthShader = resourceManager->getShader("shadowDepthShader");
        // simpleDepthShader->use();
        // simpleDepthShader->setMat4("lightSpaceMatrix", lightSpaceMatrix);
    }

    void DepthMapRenderer::beforeRender(const int index) const {
        glDisable(GL_BLEND);
        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        // Ensure depth-only framebuffer is configured properly without affecting default FBO
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
        // shader->setMat4("view", camera->getViewMatrix());
        // shader->setVec3("viewPos", camera->getPosition());
        // shader->setVec3("lightPos", lightPos);
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
        for(const auto & lightSpaceMatrice : lightSpaceMatrices) {
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

    std::vector<glm::mat4> DepthMapRenderer::computeLightSpaceMatrix(shared_ptr<DirectionalLight> &light, glm::vec3 lightTarget,
                                                                     glm::vec3 sceneMin, glm::vec3 sceneMax) {
        // glm::mat4 lightView = glm::lookAt(light.get()->getPosition(), lightTarget, glm::vec3(0,1,0));
        // glm::vec3 corners[8] = {
        //     {sceneMin.x, sceneMin.y, sceneMin.z},
        //     {sceneMax.x, sceneMin.y, sceneMin.z},
        //     {sceneMin.x, sceneMax.y, sceneMin.z},
        //     {sceneMax.x, sceneMax.y, sceneMin.z},
        //     {sceneMin.x, sceneMin.y, sceneMax.z},
        //     {sceneMax.x, sceneMin.y, sceneMax.z},
        //     {sceneMin.x, sceneMax.y, sceneMax.z},
        //     {sceneMax.x, sceneMax.y, sceneMax.z}
        // };
        //
        // glm::vec3 lightMin( FLT_MAX);
        // glm::vec3 lightMax(-FLT_MAX);
        //
        // for (auto corner : corners) {
        //     glm::vec4 trf = lightView * glm::vec4(corner, 1.0f);
        //     lightMin = glm::min(lightMin, glm::vec3(trf));
        //     lightMax = glm::max(lightMax, glm::vec3(trf));
        // }
        //
        // float near_plane = -lightMax.z;
        // float far_plane  = -lightMin.z;
        //
        // auto lightProjection = glm::ortho(
        //     lightMin.x, lightMax.x,
        //     lightMin.y, lightMax.y,
        //     near_plane, far_plane
        // );
        //
        // lightSpaceMatrix = lightProjection * lightView;
        //
        // const auto simpleDepthShader = resourceManager->getShader("shadowDepthShader");
        // simpleDepthShader->use();
        // simpleDepthShader->setMat4("lightSpaceMatrix", lightSpaceMatrix);

        lightSpaceMatrices.clear();
        lightSpaceMatrices.resize(NUM_CASCADES);

        float cameraNear = 0.1;
        float cameraFar  = 1000.0f;
        const glm::mat4 lightView = glm::lookAt(light->getPosition(), lightTarget, glm::vec3(0,1,0));

        for(int i=0; i<NUM_CASCADES; ++i)
        {
            float nearPlane = (i == 0) ? cameraNear : cameraNear + cascadeSplits[i-1] * (cameraFar - cameraNear);
            float farPlane  = cameraNear + cascadeSplits[i] * (cameraFar - cameraNear);

            // 8 rohů slice frustum
            auto frustumCorners = getFrustumCornersWorldSpace(projection, camera->getViewMatrix(), nearPlane, farPlane);

            // spočítat střed slice
            glm::vec3 center(0.0f);
            for(auto& corner : frustumCorners) center += corner;
            center /= float(frustumCorners.size());

            // auto lightDir = glm::normalize(light->getDirection());

            // počítat ortho bounding box
            float minX = +FLT_MAX, maxX = -FLT_MAX;
            float minY = +FLT_MAX, maxY = -FLT_MAX;
            float minZ = +FLT_MAX, maxZ = -FLT_MAX;

            for(auto& corner : frustumCorners)
            {
                glm::vec4 trf = lightView * glm::vec4(corner,1.0f);
                minX = std::min(minX, trf.x);
                maxX = std::max(maxX, trf.x);
                minY = std::min(minY, trf.y);
                maxY = std::max(maxY, trf.y);
                minZ = std::min(minZ, trf.z);
                maxZ = std::max(maxZ, trf.z);
            }

            // Z musí být obrácené jako v původním kódu
            float near_plane_light = -maxZ;
            float far_plane_light  = -minZ;

            lightSpaceMatrices[i] = glm::ortho(-1.0f, 3.5f, -1.0f, 3.5f, near_plane_light, far_plane_light) * lightView;
        }

        return lightSpaceMatrices;
    }

    std::vector<glm::mat4> DepthMapRenderer::computeLightSpaceMatrixForPlane(
        shared_ptr<DirectionalLight> &light,
        const glm::vec3 &planeCenter,
        float planeWidth,
        float planeHeight
    ) {
        lightSpaceMatrices.clear();
        lightSpaceMatrices.resize(NUM_CASCADES);

        // Light view matrix
        glm::mat4 lightView = glm::lookAt(light->getPosition(), planeCenter, glm::vec3(0, 1, 0));

        // Bounding box plane v lokálních souřadnicích
        glm::vec3 planeMin(-planeWidth / 2.0f, 0.0f, -planeHeight / 2.0f);
        glm::vec3 planeMax(planeWidth / 2.0f, 0.0f, planeHeight / 2.0f);

        // Transform do světla
        glm::vec3 lightMin(+FLT_MAX), lightMax(-FLT_MAX);

        std::vector<glm::vec3> corners = {
            {planeMin.x, planeMin.y, planeMin.z},
            {planeMax.x, planeMin.y, planeMin.z},
            {planeMin.x, planeMax.y, planeMin.z},
            {planeMax.x, planeMax.y, planeMin.z},
            {planeMin.x, planeMin.y, planeMax.z},
            {planeMax.x, planeMin.y, planeMax.z},
            {planeMin.x, planeMax.y, planeMax.z},
            {planeMax.x, planeMax.y, planeMax.z}
        };

        for (auto &corner: corners) {
            glm::vec4 trf = lightView * glm::vec4(corner + planeCenter, 1.0f);
            lightMin = glm::min(lightMin, glm::vec3(trf));
            lightMax = glm::max(lightMax, glm::vec3(trf));
        }

        // Jediná ortho projekce pro plane (jedna cascade)
        glm::mat4 lightProjection = glm::ortho(
            lightMin.x, lightMax.x,
            lightMin.y, lightMax.y,
            -lightMax.z - 1.0f, // lehce posunout near/far
            -lightMin.z + 1.0f
        );

        // Nastavení pro všechny kaskády (pokud používáš NUM_CASCADES)
        for (int i = 0; i < NUM_CASCADES; ++i) {
            lightSpaceMatrices[i] = lightProjection * lightView;
        }

        return lightSpaceMatrices;
    }

    std::vector<glm::vec3> DepthMapRenderer::getFrustumCornersWorldSpace(
        const glm::mat4 &proj,
        const glm::mat4 &view,
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

    void DepthMapRenderer::afterRender() {
        // auto debugDepthQuad = resourceManager->getShader("debugQuadShader");
        // debugDepthQuad->use();
        // debugDepthQuad->setFloat("near_plane", 1.0f);
        // debugDepthQuad->setFloat("far_plane", 7.5f);
        // glActiveTexture(GL_TEXTURE0);
        // glBindTexture(GL_TEXTURE_2D_ARRAY, depthMap);
        // renderQuad();
    }

    void DepthMapRenderer::renderQuad() {
        if (quadVAO == 0) {
            constexpr float quadVertices[] = {
                // positions        // texture Coords
                -1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
                -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
                1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
                1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
            };
            // setup plane VAO
            glGenVertexArrays(1, &quadVAO);
            glGenBuffers(1, &quadVBO);
            glBindVertexArray(quadVAO);
            glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), static_cast<void *>(nullptr));
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                                  reinterpret_cast<void *>(3 * sizeof(float)));
        }
        glBindVertexArray(quadVAO);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindVertexArray(0);
    }
} // Renderer
