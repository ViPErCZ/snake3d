#include "SceneHud.h"

#include <cstdio>

#include <snake3d/Renderer/Opengl/Material/2D/LabelSettings.h>
#include <snake3d/Renderer/Opengl/Scene/Scene.h>

using namespace std;
using namespace Material;
using namespace Model;
using namespace Uniform;
using namespace Manager;
using namespace Tools;

namespace Scenes {
    SceneHud::SceneHud(Scene &parent,
                       std::shared_ptr<Tools::ContextState> contextState,
                       std::shared_ptr<Manager::ResourceManager> resourceManager,
                       const int width, const int height)
        : parent(parent),
          contextState(std::move(contextState)),
          resourceManager(std::move(resourceManager)),
          width(width),
          height(height) {}

    void SceneHud::initLabels() {
        const auto shader = resourceManager->getShader("textShader");
        const auto font = std::make_shared<Font>("Assets/Fonts/OCRAEXT.TTF", 26);
        const auto settings = std::make_shared<Material::LabelSettings>(font);
        const auto label = std::make_shared<LabelNode2D>("Press start I, K or L...", shader, settings);
        label->alignVerticalCenter(static_cast<float>(width), static_cast<float>(height));

        labelFadeOutUniform = std::make_shared<Uniform::FadeOutUniform>();
        const auto shaderMaterial = std::make_shared<Material::ShaderMaterial>(shader);
        shaderMaterial->setUniform("alpha", labelFadeOutUniform);
        shaderMaterial->setUniform("textColor", glm::vec3(1.0f));
        shaderMaterial->setUniform("textTexture", 0);
        labelFadeOutUniform->setFinishedCallback([this]() {
            helpText->setVisible(false);
        });

        label->setMaterial(shaderMaterial);
        helpText = std::make_shared<MeshNode2D>(contextState, label, resourceManager);

        tilesCounterText = std::make_shared<LabelNode2D>("", shader, settings);
        counterFadeInUniform = std::make_shared<Uniform::FadeInUniform>();
        const auto shaderMaterial2 = std::make_shared<Material::ShaderMaterial>(shader);
        shaderMaterial2->setUniform("alpha", counterFadeInUniform);
        shaderMaterial2->setUniform("textColor", glm::vec3(1.0f));
        shaderMaterial2->setUniform("textTexture", 0);

        tilesCounterText->setMaterial(shaderMaterial2);
        tilesCounterNode = std::make_shared<MeshNode2D>(contextState, tilesCounterText, resourceManager);
        tilesCounterNode->setVisible(false);

        parent.addMeshNode2D(helpText);
        parent.addMeshNode2D(tilesCounterNode);
    }

    void SceneHud::initRadar() {
        radarFadeInUniform = std::make_shared<Uniform::FadeInUniform>();
        radarFadeInUniform->setStep(5.0f);
        radarExpansionIn = std::make_shared<Material::ShaderMaterial>(resourceManager->getShader("quadCorner"));
        radarExpansionIn->setUniform("quadSize", glm::vec2(200, 200));
        radarExpansionIn->setUniform("borderColor", glm::vec3(1.0, 0.0, 0.0));
        radarExpansionIn->setUniform("borderWidth", 11.9f);
        radarExpansionIn->setUniform("radius", 8.0f);
        radarExpansionIn->setUniform("expansion", radarFadeInUniform);
        radarFadeInUniform->setFinishedCallback([this]() {
            radarMeshNode->showItems();
            if (!coinVisibleOnRadar) {
                radarMeshNode->hideItem("coin");
            }
        });
        radarFadeInUniform->start();

        radarFadeOutUniform = std::make_shared<Uniform::FadeOutUniform>();
        radarFadeOutUniform->setStep(5.0f);
        radarExpansionOut = std::make_shared<Material::ShaderMaterial>(resourceManager->getShader("quadCorner"));
        radarExpansionOut->setUniform("quadSize", glm::vec2(200, 200));
        radarExpansionOut->setUniform("borderColor", glm::vec3(1.0, 0.0, 0.0));
        radarExpansionOut->setUniform("borderWidth", 11.9f);
        radarExpansionOut->setUniform("radius", 8.0f);
        radarExpansionOut->setUniform("expansion", radarFadeOutUniform);

        radarNode = std::make_shared<QuadNode2D>(220, 220, nullptr);
        radarNode->setColor(glm::vec3(0.0f, 0.0f, 0.0f));
        radarNode->setMaterial(radarExpansionIn);
        radarMeshNode = std::make_shared<RadarMeshNode2D>(contextState, radarNode, resourceManager);
        radarNode->setBlending(Blending::Translucent);
        radarMeshNode->setPosition({width - 240 + 100, 30.0 + 110, 0.0});
        radarMeshNode->hideItems();

        parent.addMeshNode2D(radarMeshNode);
    }

    void SceneHud::resize(const int width, const int height) {
        this->width = width;
        this->height = height;
        if (radarMeshNode) {
            radarMeshNode->setPosition({width - 240 + 100, 30.0f + 110, 0.0f});
        }
    }

    void SceneHud::setLevelText(const int level, const int eatCounter, const int lives, const int maxPoint) {
        if (!tilesCounterText) {
            return;
        }
        char buff[100];
        snprintf(buff, sizeof(buff),
                 "%s %d, %s %d, %s %d",
                 "Level:", level,
                 "Lives:", lives,
                 "Points left:", maxPoint - eatCounter);
        tilesCounterText->setText(buff);
    }

    void SceneHud::setHelpVisible(const bool visible) const {
        if (helpText) {
            helpText->setVisible(visible);
        }
    }

    void SceneHud::setTilesCounterVisible(const bool visible) const {
        if (tilesCounterNode) {
            tilesCounterNode->setVisible(visible);
        }
    }

    bool SceneHud::isHelpVisible() const {
        return helpText && helpText->isVisible();
    }

    void SceneHud::triggerLabelFadeOut() const {
        if (labelFadeOutUniform && labelFadeOutUniform->getAlpha() != 0.0f) {
            labelFadeOutUniform->start();
        }
    }

    void SceneHud::resetLabelFade() const {
        if (labelFadeOutUniform) {
            labelFadeOutUniform->setAlpha(1.0f);
        }
    }

    void SceneHud::triggerCounterFadeIn() const {
        if (counterFadeInUniform && counterFadeInUniform->getAlpha() != 1.0f) {
            counterFadeInUniform->start();
        }
    }

    void SceneHud::clearRadarItems() const {
        if (radarMeshNode) {
            radarMeshNode->clearItems();
        }
    }

    void SceneHud::addRadarItem(const std::shared_ptr<MeshNode3D> &item, const glm::vec3 &color, const std::string &name) const {
        if (radarMeshNode) {
            radarMeshNode->addItem(item, color, name);
        }
    }

    void SceneHud::setCoinVisible(const bool visible) {
        coinVisibleOnRadar = visible;
        if (!radarMeshNode) {
            return;
        }
        if (visible) {
            radarMeshNode->showItem("coin");
        } else {
            radarMeshNode->hideItem("coin");
        }
    }

    void SceneHud::setRadarVisible(const bool visible) const {
        if (radarMeshNode) {
            radarMeshNode->setVisible(visible);
        }
    }

    bool SceneHud::isRadarVisible() const {
        return radarMeshNode && radarMeshNode->isVisible();
    }

    void SceneHud::toggleRadar() const {
        if (!radarMeshNode) {
            return;
        }
        if (radarMeshNode->isVisible()) {
            radarNode->setMaterial(radarExpansionOut);
            radarFadeOutUniform->start();
            radarMeshNode->hideItems();
        } else {
            radarFadeOutUniform->setAlpha(1.0);
            radarMeshNode->setVisible(true);
            radarNode->setMaterial(radarExpansionIn);
            radarFadeInUniform->start();
        }
    }

    void SceneHud::hideRadarItems() const {
        if (radarMeshNode) {
            radarMeshNode->hideItems();
        }
    }

    void SceneHud::tickRadarFade() const {
        if (radarMeshNode && radarMeshNode->isVisible() && radarFadeOutUniform &&
            radarFadeOutUniform->getAlpha() <= 0) {
            radarMeshNode->setVisible(false);
        }
    }

    void SceneHud::saveVisibility() {
        if (savedState) {
            return;
        }
        savedHelpVisible = helpText && helpText->isVisible();
        savedTilesVisible = tilesCounterNode && tilesCounterNode->isVisible();
        savedRadarVisible = radarMeshNode && radarMeshNode->isVisible();

        if (helpText) {
            helpText->setVisible(false);
        }
        if (tilesCounterNode) {
            tilesCounterNode->setVisible(false);
        }
        if (radarMeshNode) {
            radarMeshNode->setVisible(false);
        }
        savedState = true;
    }

    void SceneHud::markRadarVisibleForRestore() {
        savedRadarVisible = true;
    }

    void SceneHud::restoreVisibility() {
        if (!savedState) {
            return;
        }
        if (helpText) {
            helpText->setVisible(savedHelpVisible);
        }
        if (tilesCounterNode) {
            tilesCounterNode->setVisible(savedTilesVisible);
        }
        if (radarMeshNode) {
            radarMeshNode->setVisible(savedRadarVisible);
        }
        savedState = false;
    }
}
