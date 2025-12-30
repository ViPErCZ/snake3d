#include "PlanarReflectionRenderer.h"
#include "../../Manager/RenderManager.h"
#include <GL/glew.h>
#include <iostream>
#include "Model/Standard/PlaneMesh.h"
#include "SkyboxRenderer.h"

namespace Renderer {
    PlanarReflectionRenderer::PlanarReflectionRenderer(const shared_ptr<ResourceManager> &resManager,
                                                       const shared_ptr<Camera> &camera,
                                                       const glm::mat4 &projection,
                                                       const int width, const int height)
        : resourceManager(resManager), camera(camera), projection(projection), width(width), height(height) {

        meshNode3DRenderer = make_unique<Node3DRenderer>(camera, projection);

        // Framebuffer setup
        glGenFramebuffers(1, &reflectionFBO);
        glBindFramebuffer(GL_FRAMEBUFFER, reflectionFBO);

        glGenTextures(1, &reflectionTexture);
        glBindTexture(GL_TEXTURE_2D, reflectionTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, reflectionTexture, 0);

        glGenRenderbuffers(1, &depthBuffer);
        glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "Reflection Framebuffer not complete!" << std::endl;

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        const auto textureRes = make_shared<TextureManager>(reflectionTexture);
        resourceManager->addTexture("PlanarReflectionTexture", textureRes);
    }

    PlanarReflectionRenderer::~PlanarReflectionRenderer() {
        glDeleteFramebuffers(1, &reflectionFBO);
        glDeleteTextures(1, &reflectionTexture);
        glDeleteRenderbuffers(1, &depthBuffer);
    }

    void PlanarReflectionRenderer::update(const vector<Scenes::RendererEntry3D> &nodes) {
        this->nodes3d = nodes;
    }

    void PlanarReflectionRenderer::updateRenderers(const vector<RendererEntry> &renderers) {
        this->renderers = renderers;
    }

    void PlanarReflectionRenderer::render3D(const float dt, const uint64_t frameId) {
        // Vypočet zrcadlené kamery
        glm::vec3 originalPos = camera->getPosition();
        glm::vec3 originalFront = camera->getFront();
        glm::vec3 originalUp = camera->getUp();
        
        // Předpokládáme rovinu z = planeZ (protože Z je nahoru)
        // Zrcadlíme pozici přes rovinu Z
        float dist = 2.0f * (originalPos.z - planeZ);
        camera->setPosition({originalPos.x, originalPos.y, originalPos.z - dist});
        
        // Zrcadlíme front vektor: X a Y zůstávají, Z se obrací
        glm::vec3 reflectedFront = originalFront;
        reflectedFront.z = -reflectedFront.z;
        camera->setFront(reflectedFront);

        // U zrcadla se UP vektor chová specificky. 
        // Pokud chceme, aby odraz vypadal přirozeně, musíme zachovat orientaci,
        // ale zrcadlit pozici a směr pohledu.
        // Pro Z jako UP: front.z se obrací, up.z se také obrací.
        // Tím se ale změní handness souřadného systému.
        glm::vec3 reflectedUp = originalUp;
        reflectedUp.z = -reflectedUp.z;
        camera->setUp(reflectedUp);

        glBindFramebuffer(GL_FRAMEBUFFER, reflectionFBO);
        glViewport(0, 0, width, height);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Černé pozadí
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Změna winding order kvůli zrcadlení
        glFrontFace(GL_CW);

        for (auto &node : nodes3d) {
            // Vykreslíme vše kromě podlahy (PlaneMesh)
            // Kontrola jména a typu
            if (node.node->getMesh()) {
                if (node.node->getMesh()->getMesh() && node.node->getMesh()->getMesh()->getName() == "PlaneMesh") {
                    continue;
                }
                if (dynamic_pointer_cast<PlaneMesh>(node.node->getMesh())) {
                    continue;
                }
            }
            meshNode3DRenderer->setRootNode(node.node);
            meshNode3DRenderer->render3D(dt, frameId);
        }

        // Vykreslíme ostatní renderery (např. oheň)
        for (auto &entry : renderers) {
            if (entry.renderer.get() == this) continue;
            // Nechceme zrcadlit SkyboxRenderer v této fázi
            if (dynamic_pointer_cast<SkyboxRenderer>(entry.renderer)) continue;
            // Nechceme zrcadlit SceneRenderer, protože nody už jsme vykreslili ručně v předchozím kroku (s filtrováním podlahy)
            if (dynamic_pointer_cast<Scenes::SceneRenderer>(entry.renderer)) continue;
            
            entry.renderer->beforeRender();
            entry.renderer->render3D(dt, frameId);
            entry.renderer->afterRender();
        }

        glFrontFace(GL_CCW);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // Obnovení původní kamery
        camera->setPosition(originalPos);
        camera->setFront(originalFront);
        camera->setUp(originalUp);
    }

    void PlanarReflectionRenderer::beforeRender() {}

    void PlanarReflectionRenderer::afterRender() {}

    void PlanarReflectionRenderer::renderShadowMap() {}

    void PlanarReflectionRenderer::setPlaneZ(float z) {
        planeZ = z;
    }
}
