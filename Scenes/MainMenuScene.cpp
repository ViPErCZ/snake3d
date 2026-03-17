#include "MainMenuScene.h"

#include <GLFW/glfw3.h>

namespace Scenes {
    namespace {
        constexpr float kButtonWidth = 360.0f;
        constexpr float kButtonHeight = 72.0f;
        constexpr float kButtonGap = 26.0f;
        constexpr float kTitleOffsetY = -245.0f;
        constexpr float kButtonsOffsetY = 40.0f;
        constexpr float kButtonFontSize = 40.0f;
        constexpr float kButtonTextYOffset = -14.0f;
        constexpr float kBorderWidth = 8.0f;
        constexpr float kBorderWidthHover = 9.0f;
        constexpr float kCornerRadius = 14.0f;
        constexpr float kBackdropAlpha = 0.45f;

        constexpr glm::vec3 kButtonBorder = {0.25f, 0.65f, 1.0f};
        constexpr glm::vec3 kButtonBorderHover = {1.0f, 0.85f, 0.28f};
        constexpr glm::vec3 kButtonText = {0.92f, 0.96f, 1.0f};
        constexpr glm::vec3 kButtonTextHover = {1.0f, 0.98f, 0.7f};
    }

    MainMenuScene::MainMenuScene(
        const shared_ptr<DirectionalLight> &directionalLight,
        const vector<shared_ptr<SpotLight>> &spotLights,
        const vector<shared_ptr<PointLight>> &pointLights,
        const shared_ptr<RenderManager> &rendererManager,
        const shared_ptr<Camera> &camera, const glm::mat4 &projection,
        const shared_ptr<ResourceManager> &rm, const int width, const int height)
        : OrbitSceneBase(directionalLight, spotLights, pointLights, rendererManager, camera, projection, rm, width, height) {
        viewportCenter = {static_cast<float>(width) * 0.5f, static_cast<float>(height) * 0.5f};
    }

    void MainMenuScene::init(const int priority) {
        OrbitSceneBase::init(priority);

        buttonFont = make_shared<Font>("Assets/Fonts/Orbitron-Regular.ttf", static_cast<int>(kButtonFontSize));
        buttonSettings = make_shared<LabelSettings>(buttonFont);
        titleTimer = make_shared<TimerUniform>(true);
        initBackdrop();

        const glm::vec2 titleCenter = {viewportCenter.x, viewportCenter.y + kTitleOffsetY};
        initTitle(titleCenter);

        const glm::vec2 buttonsCenter = {viewportCenter.x, viewportCenter.y + kButtonsOffsetY};
        initButtons(buttonsCenter);
    }

    void MainMenuScene::initBackdrop() {
        const auto quad = make_shared<QuadNode2D>(static_cast<float>(width), static_cast<float>(height));
        quad->setBlending(Blending::Translucent);
        quad->setDepthTest(false);
        quad->setDepthWrite(false);

        const auto material = make_shared<ShaderMaterial>(resourceManager->getShader("basic2d"));
        material->setUniform("color", glm::vec3(0.0f, 0.0f, 0.0f));
        material->setUniform("alpha", kBackdropAlpha);
        material->setUniform("useMaterial", true);
        quad->setMaterial(material);

        const auto node = make_shared<MeshNode2D>(contextState, quad, resourceManager);
        node->setPosition({viewportCenter.x, viewportCenter.y, 0.0f});
        addMeshNode2D(node, -10);
    }

    void MainMenuScene::update() {
        OrbitSceneBase::update();
        updateHoverState();
    }

    void MainMenuScene::setCursorPosition(const glm::vec2 &position) {
        cursorScreenPos = position;
        cursorValid = true;
    }

    MainMenuScene::MenuAction MainMenuScene::handleMouseButton(const int button, const int action) {
        if (button != GLFW_MOUSE_BUTTON_LEFT || action != GLFW_PRESS) {
            return MenuAction::None;
        }

        if (startButton.hovered) {
            return primaryAction == PrimaryAction::Start ? MenuAction::Start : MenuAction::Resume;
        }
        if (quitButton.hovered) {
            return MenuAction::Quit;
        }
        return MenuAction::None;
    }

    void MainMenuScene::setPrimaryAction(const PrimaryAction action) {
        if (primaryAction == action) {
            return;
        }
        primaryAction = action;

        if (!startButton.labelMesh || !startButton.labelNode) {
            return;
        }

        const std::string labelText = (primaryAction == PrimaryAction::Start) ? "Start" : "Resume";
        startButton.labelMesh->setText(labelText);
        startButton.labelMesh->alignVerticalCenter(static_cast<float>(width), static_cast<float>(height));
        startButton.labelNode->setPosition({startButton.labelOffset.x, startButton.labelOffset.y + kButtonTextYOffset, 0.0f});
    }

    void MainMenuScene::initTitle(const glm::vec2 &center) {
        constexpr float kTitleImageWidth = 1317.0f;
        constexpr float kTitleImageHeight = 246.0f;
        constexpr float kTitleImageScale = 0.65f;

        const auto titleShader = resourceManager->getShader("titleImageFx");
        const auto titleTexture = resourceManager->getTexture("snake3d_title_base.png");

        const auto image = make_shared<ImageNode2D>(kTitleImageWidth * kTitleImageScale,
            kTitleImageHeight * kTitleImageScale, titleShader, titleTexture);
        image->setBlending(Blending::Translucent);
        image->setDepthTest(false);
        image->setDepthWrite(false);

        const auto material = make_shared<ShaderMaterial>(titleShader);
        material->setUniform("textureMap", 0);
        material->setUniform("faceColor", glm::vec3(1.0f, 1.0f, 1.0f));
        material->setUniform("glowColor", glm::vec3(2.6f, 0.95f, 0.12f));
        material->setUniform("glowColor2", glm::vec3(1.9f, 0.65f, 0.08f));
        material->setUniform("depthColor", glm::vec3(0.6f, 0.6f, 0.6f));
        material->setUniform("depthOffset", glm::vec2(0.0018f, -0.0022f));
        material->setUniform("time", titleTimer);
        material->setUniform("alpha", 1.0f);
        image->setMaterial(material);

        const auto node = make_shared<MeshNode2D>(contextState, image, resourceManager);
        node->setPosition({center.x, center.y, 0.0f});
        addMeshNode2D(node, 40);
    }

    void MainMenuScene::initButtons(const glm::vec2 &center) {
        constexpr glm::vec2 size = {kButtonWidth, kButtonHeight};
        constexpr float offset = kButtonGap * 0.5f + kButtonHeight * 0.5f;

        const glm::vec2 startCenter = {center.x, center.y - offset};
        const glm::vec2 quitCenter = {center.x, center.y + offset};

        startButton = buildButton("Start", startCenter, size);
        quitButton = buildButton("Quit", quitCenter, size);
    }

    MainMenuScene::MenuButton MainMenuScene::buildButton(const std::string &text, const glm::vec2 &center,
                                                         const glm::vec2 &size) {
        MenuButton button{};
        button.center = center;
        button.size = size;
        button.labelOffset = center - viewportCenter;

        const auto quad = make_shared<QuadNode2D>(size.x, size.y);
        quad->setBlending(Blending::Translucent);
        quad->setDepthTest(false);
        quad->setDepthWrite(false);

        const auto backgroundMaterial = make_shared<ShaderMaterial>(resourceManager->getShader("quadCorner"));
        backgroundMaterial->setUniform("quadSize", size);
        backgroundMaterial->setUniform("borderWidth", kBorderWidth);
        backgroundMaterial->setUniform("borderColor", kButtonBorder);
        backgroundMaterial->setUniform("radius", kCornerRadius);
        quad->setMaterial(backgroundMaterial);

        const auto backgroundNode = make_shared<MeshNode2D>(contextState, quad, resourceManager);
        backgroundNode->setPosition({center.x, center.y, 0.0f});
        addMeshNode2D(backgroundNode, 60);

        const auto textShader = resourceManager->getShader("textShader");
        auto label = make_shared<LabelNode2D>(text, textShader, buttonSettings);
        label->alignVerticalCenter(static_cast<float>(width), static_cast<float>(height));

        const auto textMaterial = make_shared<ShaderMaterial>(textShader);
        textMaterial->setUniform("textColor", kButtonText);
        textMaterial->setUniform("textTexture", 0);
        textMaterial->setUniform("alpha", 1.0f);
        label->setMaterial(textMaterial);

        auto labelNode = make_shared<MeshNode2D>(contextState, label, resourceManager);
        labelNode->setPosition({button.labelOffset.x, button.labelOffset.y + kButtonTextYOffset, 0.0f});
        addMeshNode2D(labelNode, 50);

        button.backgroundNode = backgroundNode;
        button.labelNode = labelNode;
        button.labelMesh = label;
        button.backgroundMaterial = backgroundMaterial;
        button.textMaterial = textMaterial;

        applyButtonStyle(button, false);
        return button;
    }

    void MainMenuScene::updateHoverState() {
        if (!cursorValid) {
            return;
        }

        const bool startHovered = hitTest(startButton);
        if (startHovered != startButton.hovered) {
            applyButtonStyle(startButton, startHovered);
            startButton.hovered = startHovered;
        }

        const bool quitHovered = hitTest(quitButton);
        if (quitHovered != quitButton.hovered) {
            applyButtonStyle(quitButton, quitHovered);
            quitButton.hovered = quitHovered;
        }
    }

    void MainMenuScene::applyButtonStyle(const MenuButton &button, const bool hovered) {
        if (!button.backgroundMaterial || !button.textMaterial) {
            return;
        }

        button.backgroundMaterial->setUniform("borderColor", hovered ? kButtonBorderHover : kButtonBorder);
        button.backgroundMaterial->setUniform("borderWidth", hovered ? kBorderWidthHover : kBorderWidth);
        button.textMaterial->setUniform("textColor", hovered ? kButtonTextHover : kButtonText);
    }

    bool MainMenuScene::hitTest(const MenuButton &button) const {
        if (!cursorValid) {
            return false;
        }
        const float halfW = button.size.x * 0.5f;
        const float halfH = button.size.y * 0.5f;
        return cursorScreenPos.x >= (button.center.x - halfW) &&
               cursorScreenPos.x <= (button.center.x + halfW) &&
               cursorScreenPos.y >= (button.center.y - halfH) &&
               cursorScreenPos.y <= (button.center.y + halfH);
    }
} // Scenes
