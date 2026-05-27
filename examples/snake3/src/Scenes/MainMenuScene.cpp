#include "MainMenuScene.h"

#include <GLFW/glfw3.h>
#include <algorithm>
#include <iostream>

#include "Resource/TextureLoader.h"
#include "Thirdparty/stbimage/stb_image.h"

using namespace Resource;

namespace Scenes {
    namespace {
        constexpr float kCursorScale = 0.9f;
        constexpr float kCursorTrailSpawnMax = 80.0f;
        constexpr float kCursorTrailSpawnPerPixel = 12.0f;
        constexpr float kCursorTrailSpawnMin = 4.0f;
        constexpr float kCursorEmitterRadius = 0.02f;
    }

    namespace {
        constexpr float kButtonWidth = 360.0f;
        constexpr float kButtonHeight = 72.0f;
        constexpr float kButtonGap = 26.0f;
        constexpr float kTitleOffsetY = -245.0f;
        constexpr float kButtonsOffsetY = 18.0f;
        constexpr float kNetworkOffsetY = 100.0f;
        constexpr float kNetworkInfoOffsetY = 140.0f;
        constexpr float kButtonFontSize = 40.0f;
        constexpr float kNetInfoFontSize = 22.0f;
        constexpr float kButtonTextYOffset = -14.0f;
        constexpr float kIpFieldWidth = kButtonWidth;
        constexpr float kIpFieldHeight = kButtonHeight;
        constexpr float kIpFieldPadding = 18.0f;
        constexpr float kBorderWidth = 8.0f;
        constexpr float kBorderWidthHover = 9.0f;
        constexpr float kCornerRadius = 14.0f;
        constexpr float kBackdropAlpha = 0.45f;
        constexpr float kStatusOffsetY = 120.0f;
        constexpr float kIpHintOffsetY = 160.0f;
        constexpr float kLocalIpOffsetY = 186.0f;

        constexpr glm::vec3 kButtonBorder = {0.25f, 0.65f, 1.0f};
        constexpr glm::vec3 kButtonBorderHover = {1.0f, 0.85f, 0.28f};
        constexpr glm::vec3 kButtonBorderDisabled = {0.18f, 0.24f, 0.32f};
        constexpr glm::vec3 kButtonText = {0.92f, 0.96f, 1.0f};
        constexpr glm::vec3 kButtonTextHover = {1.0f, 0.98f, 0.7f};
        constexpr glm::vec3 kButtonTextDisabled = {0.46f, 0.52f, 0.6f};
        constexpr glm::vec3 kIpTextColor = {0.88f, 0.94f, 1.0f};
        constexpr glm::vec3 kIpTextDisabled = {0.42f, 0.48f, 0.56f};
        constexpr glm::vec3 kNetHintColor = {0.7f, 0.8f, 0.95f};
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
        netInfoFont = make_shared<Font>("Assets/Fonts/Orbitron-Regular.ttf", static_cast<int>(kNetInfoFontSize));
        netInfoSettings = make_shared<LabelSettings>(netInfoFont);
        titleTimer = make_shared<TimerUniform>(true);
        initBackdrop();

        const glm::vec2 titleCenter = {viewportCenter.x, viewportCenter.y + kTitleOffsetY};
        initTitle(titleCenter);

        const glm::vec2 buttonsCenter = {viewportCenter.x, viewportCenter.y + kButtonsOffsetY};
        initButtons(buttonsCenter);
        initCursor();
        setMenuView(MenuView::Main);
        updateIpLabel();
    }

    void MainMenuScene::initBackdrop() {
        const auto quad = make_shared<QuadNode2D>(10000.0f, 10000.0f);
        quad->setBlending(Blending::Translucent);
        quad->setDepthTest(false);
        quad->setDepthWrite(false);

        const auto material = make_shared<ShaderMaterial>(resourceManager->getShader("basic2d"));
        material->setUniform("color", glm::vec3(0.0f, 0.0f, 0.0f));
        material->setUniform("alpha", kBackdropAlpha);
        material->setUniform("useMaterial", true);
        quad->setMaterial(material);

        backdropNode = make_shared<MeshNode2D>(contextState, quad, resourceManager);
        backdropNode->setPosition({viewportCenter.x, viewportCenter.y, 0.0f});
        addMeshNode2D(backdropNode, -10);
    }

    void MainMenuScene::update() {
        OrbitSceneBase::update();
        updateHoverState();
        if (menuView == MenuView::Network) {
            const double t = glfwGetTime();
            const bool nextCaret = ipInputActive && (static_cast<int>(t * 2.0) % 2 == 0);
            if (nextCaret != caretVisible) {
                caretVisible = nextCaret;
                updateIpLabel();
            }
        }
        updateCursor();
    }

    void MainMenuScene::setCursorPosition(const glm::vec2 &position) {
        cursorScreenPos = position;
        cursorValid = true;
        if (!hasCursorLastPos) {
            lastCursorScreenPos = position;
        }
    }

    MainMenuScene::MenuAction MainMenuScene::handleMouseButton(const int button, const int action) {
        if (button != GLFW_MOUSE_BUTTON_LEFT || action != GLFW_PRESS) {
            return MenuAction::None;
        }

        if (menuView == MenuView::Main) {
            if (networkButton.hovered) {
                if (networkSessionState != NetworkSessionState::Idle) {
                    return MenuAction::Disconnect;
                }
                setMenuView(MenuView::Network);
                return MenuAction::None;
            }
            if (startButton.hovered) {
                return primaryAction == PrimaryAction::Start ? MenuAction::Start : MenuAction::Resume;
            }
            if (newGameButton.hovered) {
                return MenuAction::NewGame;
            }
            if (quitButton.hovered) {
                return MenuAction::Quit;
            }
            return MenuAction::None;
        }

        if (networkSessionState == NetworkSessionState::Idle && ipField.hovered) {
            ipInputActive = true;
            caretIndex = static_cast<int>(joinIp.size());
            updateIpLabel();
            return MenuAction::None;
        }

        ipInputActive = false;
        updateIpLabel();

        if (hostButton.hovered) {
            if (networkSessionState != NetworkSessionState::Idle) {
                return MenuAction::None;
            }
            return MenuAction::Host;
        }
        if (joinButton.hovered) {
            return networkSessionState == NetworkSessionState::Idle ? MenuAction::Join : MenuAction::Disconnect;
        }
        if (backButton.hovered) {
            setMenuView(MenuView::Main);
            return MenuAction::None;
        }
        return MenuAction::None;
    }

    void MainMenuScene::setPrimaryAction(const PrimaryAction action) {
        if (primaryAction == action) {
            return;
        }
        primaryAction = action;

        if (menuView == MenuView::Main) {
            setButtonVisible(networkButton, primaryAction == PrimaryAction::Start);
            setButtonVisible(newGameButton, primaryAction == PrimaryAction::Resume);
        }

        if (!startButton.labelMesh || !startButton.labelNode) {
            return;
        }

        const std::string labelText = (primaryAction == PrimaryAction::Start) ? "Start" : "Resume";
        startButton.labelMesh->setText(labelText);
        startButton.labelMesh->alignVerticalCenter(static_cast<float>(width), static_cast<float>(height));
        startButton.labelNode->setPosition({startButton.labelOffset.x, startButton.labelOffset.y + kButtonTextYOffset, 0.0f});
    }

    void MainMenuScene::handleKeyInput(const int key, const int action) {
        if (menuView != MenuView::Network || action != GLFW_PRESS || networkSessionState != NetworkSessionState::Idle) {
            return;
        }

        if (key == GLFW_KEY_ESCAPE) {
            setMenuView(MenuView::Main);
            return;
        }

        if (!ipInputActive) {
            return;
        }

        if (key == GLFW_KEY_ENTER || key == GLFW_KEY_KP_ENTER) {
            joinRequested = true;
            return;
        }

        if (key == GLFW_KEY_LEFT) {
            if (caretIndex > 0) {
                caretIndex--;
                updateIpLabel();
            }
            return;
        }
        if (key == GLFW_KEY_RIGHT) {
            if (caretIndex < static_cast<int>(joinIp.size())) {
                caretIndex++;
                updateIpLabel();
            }
            return;
        }

        if (key == GLFW_KEY_BACKSPACE) {
            if (caretIndex > 0 && !joinIp.empty()) {
                joinIp.erase(static_cast<size_t>(caretIndex - 1), 1);
                caretIndex--;
                updateIpLabel();
            }
            return;
        }
        if (key == GLFW_KEY_DELETE) {
            if (caretIndex < static_cast<int>(joinIp.size())) {
                joinIp.erase(static_cast<size_t>(caretIndex), 1);
                updateIpLabel();
            }
            return;
        }

        auto appendChar = [&](const char c) {
            if (joinIp.size() >= 31) {
                return;
            }
            joinIp.insert(static_cast<size_t>(caretIndex), 1, c);
            caretIndex++;
            updateIpLabel();
        };

        if (key >= GLFW_KEY_0 && key <= GLFW_KEY_9) {
            appendChar(static_cast<char>('0' + (key - GLFW_KEY_0)));
        } else if (key >= GLFW_KEY_KP_0 && key <= GLFW_KEY_KP_9) {
            appendChar(static_cast<char>('0' + (key - GLFW_KEY_KP_0)));
        } else if (key == GLFW_KEY_PERIOD || key == GLFW_KEY_KP_DECIMAL) {
            appendChar('.');
        }
    }

    bool MainMenuScene::consumeJoinRequest() {
        if (!joinRequested) {
            return false;
        }
        joinRequested = false;
        return true;
    }

    void MainMenuScene::setLocalIp(const std::string &ip) {
        localIpLabel = "IP: " + ip;
        updateNetworkLabels();
    }

    void MainMenuScene::setNetworkStatus(std::string status) {
        networkStatus = std::move(status);
        updateNetworkLabels();
    }

    void MainMenuScene::setNetworkSessionState(const NetworkSessionState state) {
        networkSessionState = state;
        setButtonText(networkButton, networkSessionState == NetworkSessionState::Idle ? "Multiplayer" : "Disconnect");
        setButtonText(joinButton, networkSessionState == NetworkSessionState::Idle ? "Connect" : "Disconnect");

        const bool hostLocked = networkSessionState == NetworkSessionState::Hosting;
        if (hostButton.backgroundMaterial && hostButton.textMaterial) {
            hostButton.backgroundMaterial->setUniform("borderColor", hostLocked ? kButtonBorderDisabled : kButtonBorder);
            hostButton.backgroundMaterial->setUniform("borderWidth", hostLocked ? kBorderWidth : kBorderWidth);
            hostButton.textMaterial->setUniform("textColor", hostLocked ? kButtonTextDisabled : kButtonText);
        }
        hostButton.hovered = false;

        const bool ipLocked = networkSessionState != NetworkSessionState::Idle;
        if (ipField.backgroundMaterial && ipField.textMaterial) {
            ipField.backgroundMaterial->setUniform("borderColor", ipLocked ? kButtonBorderDisabled : kButtonBorder);
            ipField.backgroundMaterial->setUniform("borderWidth", kBorderWidth);
            ipField.textMaterial->setUniform("textColor", ipLocked ? kIpTextDisabled : kIpTextColor);
        }
        ipField.hovered = false;
        if (ipLocked) {
            ipInputActive = false;
            caretVisible = false;
        }

        if (networkSessionState == NetworkSessionState::Idle && menuView == MenuView::Network) {
            updateIpLabel();
            updateNetworkLabels();
        }
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

        titleNode = make_shared<MeshNode2D>(contextState, image, resourceManager);
        titleNode->setPosition({center.x, center.y, 0.0f});
        addMeshNode2D(titleNode, 40);
    }

    void MainMenuScene::initButtons(const glm::vec2 &center) {
        constexpr glm::vec2 size = {kButtonWidth, kButtonHeight};
        constexpr float step = kButtonHeight + kButtonGap;
        const glm::vec2 mainCenter = center;
        const glm::vec2 networkCenter = {center.x, center.y + kNetworkOffsetY};
        networkInfoBase = {center.x, center.y + kNetworkInfoOffsetY + step * 0.9f};

        const glm::vec2 startCenter = {mainCenter.x, mainCenter.y - step * 0.0f};
        const glm::vec2 networkCenterBtn = {mainCenter.x, mainCenter.y + step * 1.0f};
        const glm::vec2 quitCenter = {mainCenter.x, mainCenter.y + step * 2.0f};

        const glm::vec2 hostCenter = {networkCenter.x, networkCenter.y - step * 1.3f};
        const glm::vec2 joinCenter = {networkCenter.x, networkCenter.y - step * 0.2f};
        const glm::vec2 ipCenter = {networkCenter.x, networkCenter.y + step * 0.9f};
        const glm::vec2 backCenter = {networkCenter.x, networkCenter.y + step * 2.0f};

        startButton = buildButton("Start", startCenter, size);
        networkButton = buildButton("Multiplayer", networkCenterBtn, size);
        newGameButton = buildButton("New Game", networkCenterBtn, size);
        quitButton = buildButton("Exit", quitCenter, size);

        hostButton = buildButton("Host", hostCenter, size);
        joinButton = buildButton("Connect", joinCenter, size);
        ipField = buildIpField(ipCenter, {kIpFieldWidth, kIpFieldHeight});
        backButton = buildButton("Back", backCenter, size);
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

        const auto labelNode = make_shared<MeshNode2D>(contextState, label, resourceManager);
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

    MainMenuScene::MenuButton MainMenuScene::buildIpField(const glm::vec2 &center, const glm::vec2 &size) {
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
        auto label = make_shared<LabelNode2D>("", textShader, buttonSettings);
        label->alignVerticalCenter(static_cast<float>(width), static_cast<float>(height));

        const auto textMaterial = make_shared<ShaderMaterial>(textShader);
        textMaterial->setUniform("textColor", kIpTextColor);
        textMaterial->setUniform("textTexture", 0);
        textMaterial->setUniform("alpha", 1.0f);
        label->setMaterial(textMaterial);

        const auto labelNode = make_shared<MeshNode2D>(contextState, label, resourceManager);
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

        if (menuView == MenuView::Main) {
            const bool startHovered = hitTest(startButton);
            if (startHovered != startButton.hovered) {
                applyButtonStyle(startButton, startHovered);
                startButton.hovered = startHovered;
            }
            const bool networkHovered = hitTest(networkButton);
            if (networkHovered != networkButton.hovered) {
                applyButtonStyle(networkButton, networkHovered);
                networkButton.hovered = networkHovered;
            }
            const bool newGameHovered = hitTest(newGameButton);
            if (newGameHovered != newGameButton.hovered) {
                applyButtonStyle(newGameButton, newGameHovered);
                newGameButton.hovered = newGameHovered;
            }
            const bool quitHovered = hitTest(quitButton);
            if (quitHovered != quitButton.hovered) {
                applyButtonStyle(quitButton, quitHovered);
                quitButton.hovered = quitHovered;
            }
            return;
        }

        const bool hostHovered = networkSessionState == NetworkSessionState::Idle && hitTest(hostButton);
        if (hostHovered != hostButton.hovered) {
            applyButtonStyle(hostButton, hostHovered);
            hostButton.hovered = hostHovered;
        }

        const bool joinHovered = hitTest(joinButton);
        if (joinHovered != joinButton.hovered) {
            applyButtonStyle(joinButton, joinHovered);
            joinButton.hovered = joinHovered;
        }

        const bool ipHovered = networkSessionState == NetworkSessionState::Idle && hitTest(ipField);
        if (ipHovered != ipField.hovered) {
            applyButtonStyle(ipField, ipHovered);
            ipField.hovered = ipHovered;
        }

        const bool backHovered = hitTest(backButton);
        if (backHovered != backButton.hovered) {
            applyButtonStyle(backButton, backHovered);
            backButton.hovered = backHovered;
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
        if (!button.backgroundNode || !button.backgroundNode->isVisible()) {
            return false;
        }
        const float halfW = button.size.x * 0.5f;
        const float halfH = button.size.y * 0.5f;
        return cursorScreenPos.x >= (button.center.x - halfW) &&
               cursorScreenPos.x <= (button.center.x + halfW) &&
               cursorScreenPos.y >= (button.center.y - halfH) &&
               cursorScreenPos.y <= (button.center.y + halfH);
    }

    void MainMenuScene::updateIpLabel() {
        if (!ipField.labelMesh || !ipField.labelNode) {
            return;
        }

        const int clampedCaret = std::max(0, std::min(caretIndex, static_cast<int>(joinIp.size())));
        const std::string caret = (ipInputActive && caretVisible) ? "|" : "";
        const std::string label = joinIp.substr(0, clampedCaret) + caret + joinIp.substr(clampedCaret);
        if (label != lastIpRender) {
            lastIpRender = label;
            ipField.labelMesh->setText(label);
            ipField.labelMesh->alignVerticalCenter(static_cast<float>(width), static_cast<float>(height));

            const float labelWidth = ipField.labelMesh->getWidth();
            const glm::vec2 base = ipField.center - viewportCenter;
            const float x = base.x - ipField.size.x * 0.5f + kIpFieldPadding + labelWidth * 0.5f;
            const float y = base.y + kButtonTextYOffset;
            ipField.labelNode->setPosition({x, y, 0.0f});
        }
    }

    void MainMenuScene::layoutButton(MenuButton &button) const {
        button.labelOffset = button.center - viewportCenter;
        if (button.backgroundNode) {
            button.backgroundNode->setPosition({button.center.x, button.center.y, 0.0f});
        }
        if (button.labelMesh) {
            button.labelMesh->alignVerticalCenter(static_cast<float>(width), static_cast<float>(height));
        }
        if (button.labelNode) {
            button.labelNode->setPosition({button.labelOffset.x, button.labelOffset.y + kButtonTextYOffset, 0.0f});
        }
    }

    void MainMenuScene::updateLayout() {
        viewportCenter = {static_cast<float>(width) * 0.5f, static_cast<float>(height) * 0.5f};

        if (backdropNode) {
            backdropNode->setPosition({viewportCenter.x, viewportCenter.y, 0.0f});
        }
        if (titleNode) {
            titleNode->setPosition({viewportCenter.x, viewportCenter.y + kTitleOffsetY, 0.0f});
        }

        constexpr float step = kButtonHeight + kButtonGap;
        const glm::vec2 mainCenter = {viewportCenter.x, viewportCenter.y + kButtonsOffsetY};
        const glm::vec2 networkCenter = {mainCenter.x, mainCenter.y + kNetworkOffsetY};
        networkInfoBase = {mainCenter.x, mainCenter.y + kNetworkInfoOffsetY + step * 0.9f};

        startButton.center = {mainCenter.x, mainCenter.y - step * 0.0f};
        networkButton.center = {mainCenter.x, mainCenter.y + step * 1.0f};
        newGameButton.center = {mainCenter.x, mainCenter.y + step * 1.0f};
        quitButton.center = {mainCenter.x, mainCenter.y + step * 2.0f};

        hostButton.center = {networkCenter.x, networkCenter.y - step * 1.3f};
        joinButton.center = {networkCenter.x, networkCenter.y - step * 0.2f};
        ipField.center = {networkCenter.x, networkCenter.y + step * 0.9f};
        backButton.center = {networkCenter.x, networkCenter.y + step * 2.0f};

        layoutButton(startButton);
        layoutButton(networkButton);
        layoutButton(newGameButton);
        layoutButton(quitButton);
        layoutButton(hostButton);
        layoutButton(joinButton);
        layoutButton(ipField);
        layoutButton(backButton);

        updateIpLabel();
        updateNetworkLabels();
    }

    void MainMenuScene::resize(const int width, const int height, const glm::mat4 &projection) {
        OrbitSceneBase::resize(width, height, projection);
        updateLayout();
        if (cursorTrail) {
            cursorTrail->setAspectRatio(static_cast<float>(width) / static_cast<float>(height));
        }
    }

    void MainMenuScene::initCursor() {
        int imgW = 0;
        int imgH = 0;
        int imgCh = 0;
        unsigned char* pixels = stbi_load("Assets/Cursors/Arrow_Rounded_Blue.png", &imgW, &imgH, &imgCh, 4);
        if (!pixels) {
            std::cerr << "Failed to load cursor texture." << std::endl;
            return;
        }

        const unsigned int textureId = TextureLoader::bindFromBufferWithoutDecode(
            pixels, true, imgW, imgH, 4);
        stbi_image_free(pixels);

        cursorTexture = std::make_shared<TextureManager>(textureId);
        cursorSize = glm::vec2(static_cast<float>(imgW), static_cast<float>(imgH)) * kCursorScale;

        const auto shader = resourceManager->getShader("cursor2d");
        cursorMesh = make_shared<ImageNode2D>(cursorSize.x, cursorSize.y, shader, cursorTexture);
        cursorMesh->setBlending(Blending::Translucent);
        cursorMesh->setDepthTest(false);
        cursorMesh->setDepthWrite(false);

        cursorNode = make_shared<MeshNode2D>(contextState, cursorMesh, resourceManager);
        addMeshNode2D(cursorNode, -10000);

        const auto trailQuad = make_shared<QuadNode2D>(0.03f, 0.03f);
        trailQuad->setBlending(Blending::Additive);
        trailQuad->setDepthTest(false);
        trailQuad->setDepthWrite(false);

        cursorTrailMaterial = make_shared<ParticleProcessMaterial>(resourceManager);
        cursorTrailMaterial->set_spawn_shape(0);
        cursorTrailMaterial->set_respawn_mode(0);
        cursorTrailMaterial->set_life_min(0.35f);
        cursorTrailMaterial->set_life_max(0.7f);
        cursorTrailMaterial->set_size_min(0.08f);
        cursorTrailMaterial->set_size_max(0.16f);
        cursorTrailMaterial->set_vel_min({-0.12f, -0.12f, 0.0f});
        cursorTrailMaterial->set_vel_max({0.12f, 0.12f, 0.0f});
        cursorTrailMaterial->set_gravity({0.0f, 0.0f, 0.0f});
        cursorTrailMaterial->set_spawn_per_frame(0.0f);
        cursorTrailMaterial->set_color_start({0.45f, 0.85f, 1.0f, 0.9f});
        cursorTrailMaterial->set_color_end({0.15f, 0.3f, 1.0f, 0.0f});
        cursorTrailMaterial->set_emitter_radius(kCursorEmitterRadius);

        cursorTrail = make_shared<GPUParticle2D>(cursorTrailMaterial, contextState, trailQuad, resourceManager, 400);
        cursorTrail->setAspectRatio(static_cast<float>(width) / static_cast<float>(height));
        cursorTrail->setRenderShader(resourceManager->getShader("particle_render_2d_trail"));
        addMeshNode2D(cursorTrail, -9000);

        cursorInitialized = true;
    }

    void MainMenuScene::updateCursor() {
        if (!cursorInitialized || !cursorNode || !cursorTrailMaterial) {
            return;
        }

        const float cursorX = cursorScreenPos.x + (cursorSize.x * 0.5f) - cursorHotspot.x;
        const float cursorY = cursorScreenPos.y + (cursorSize.y * 0.5f) - cursorHotspot.y;
        cursorNode->setPosition({cursorX, cursorY, 0.0f});

        const glm::vec2 delta = cursorScreenPos - lastCursorScreenPos;
        const float moveLen = glm::length(delta);
        if (hasCursorLastPos && moveLen > 0.5f) {
            const float emitX = cursorScreenPos.x + cursorSize.x - cursorHotspot.x;
            const float emitY = cursorScreenPos.y + cursorSize.y - cursorHotspot.y;
            const float ndcX = (emitX / static_cast<float>(width)) * 2.0f - 1.0f;
            const float ndcY = 1.0f - (emitY / static_cast<float>(height)) * 2.0f;
            cursorTrailMaterial->set_emitter_pos({ndcX, ndcY, 0.0f});
            const float spawnRate = std::clamp(
                moveLen * kCursorTrailSpawnPerPixel,
                kCursorTrailSpawnMin,
                kCursorTrailSpawnMax);
            cursorTrailMaterial->set_spawn_per_frame(spawnRate);
        } else {
            cursorTrailMaterial->set_spawn_per_frame(0.0f);
        }

        lastCursorScreenPos = cursorScreenPos;
        hasCursorLastPos = true;
    }

    void MainMenuScene::setMenuView(const MenuView view) {
        menuView = view;
        ipInputActive = false;
        joinRequested = false;
        updateIpLabel();
        updateNetworkLabels();

        const bool mainVisible = (menuView == MenuView::Main);
        setButtonVisible(startButton, mainVisible);
        setButtonVisible(networkButton, mainVisible && primaryAction == PrimaryAction::Start);
        setButtonVisible(newGameButton, mainVisible && primaryAction == PrimaryAction::Resume);
        setButtonVisible(quitButton, mainVisible);

        const bool netVisible = (menuView == MenuView::Network);
        setButtonVisible(hostButton, netVisible);
        setButtonVisible(joinButton, netVisible);
        setButtonVisible(ipField, netVisible);
        setButtonVisible(backButton, netVisible);
        if (localIpNode) {
            localIpNode->setVisible(netVisible);
        }
        if (ipHintNode) {
            ipHintNode->setVisible(netVisible);
        }
        if (netStatusNode) {
            netStatusNode->setVisible(netVisible);
        }
    }

    void MainMenuScene::setButtonVisible(const MenuButton &button, const bool visible) const {
        if (button.backgroundNode) {
            button.backgroundNode->setVisible(visible);
        }
        if (button.labelNode) {
            button.labelNode->setVisible(visible);
        }
    }

    void MainMenuScene::setButtonText(const MenuButton &button, const std::string &text) const {
        if (!button.labelMesh || !button.labelNode) {
            return;
        }

        button.labelMesh->setText(text);
        button.labelMesh->alignVerticalCenter(static_cast<float>(width), static_cast<float>(height));
        button.labelNode->setPosition({button.labelOffset.x, button.labelOffset.y + kButtonTextYOffset, 0.0f});
    }

    void MainMenuScene::updateNetworkLabels() {
        if (!ipHintMesh) {
            const auto textShader = resourceManager->getShader("textShader");
            ipHintMesh = make_shared<LabelNode2D>("SERVER IP", textShader, netInfoSettings);
            ipHintMesh->alignVerticalCenter(static_cast<float>(width), static_cast<float>(height));

            ipHintMaterial = make_shared<ShaderMaterial>(textShader);
            ipHintMaterial->setUniform("textColor", kNetHintColor);
            ipHintMaterial->setUniform("textTexture", 0);
            ipHintMaterial->setUniform("alpha", 1.0f);
            ipHintMesh->setMaterial(ipHintMaterial);

            ipHintNode = make_shared<MeshNode2D>(contextState, ipHintMesh, resourceManager);
            addMeshNode2D(ipHintNode, 70);
        }

        if (!localIpMesh) {
            const auto textShader = resourceManager->getShader("textShader");
            localIpMesh = make_shared<LabelNode2D>("", textShader, netInfoSettings);
            localIpMesh->alignVerticalCenter(static_cast<float>(width), static_cast<float>(height));

            localIpMaterial = make_shared<ShaderMaterial>(textShader);
            localIpMaterial->setUniform("textColor", kNetHintColor);
            localIpMaterial->setUniform("textTexture", 0);
            localIpMaterial->setUniform("alpha", 1.0f);
            localIpMesh->setMaterial(localIpMaterial);

            localIpNode = make_shared<MeshNode2D>(contextState, localIpMesh, resourceManager);
            addMeshNode2D(localIpNode, 70);
        }

        if (!netStatusMesh) {
            const auto textShader = resourceManager->getShader("textShader");
            netStatusMesh = make_shared<LabelNode2D>("", textShader, netInfoSettings);
            netStatusMesh->alignVerticalCenter(static_cast<float>(width), static_cast<float>(height));

            netStatusMaterial = make_shared<ShaderMaterial>(textShader);
            netStatusMaterial->setUniform("textColor", kNetHintColor);
            netStatusMaterial->setUniform("textTexture", 0);
            netStatusMaterial->setUniform("alpha", 1.0f);
            netStatusMesh->setMaterial(netStatusMaterial);

            netStatusNode = make_shared<MeshNode2D>(contextState, netStatusMesh, resourceManager);
            addMeshNode2D(netStatusNode, 70);
        }

        ipHintMesh->setText("SERVER IP");
        ipHintMesh->alignVerticalCenter(static_cast<float>(width), static_cast<float>(height));
        localIpMesh->setText("LOCAL " + localIpLabel);
        localIpMesh->alignVerticalCenter(static_cast<float>(width), static_cast<float>(height));
        netStatusMesh->setText(networkStatus);
        netStatusMesh->alignVerticalCenter(static_cast<float>(width), static_cast<float>(height));

        const glm::vec2 base = networkInfoBase - viewportCenter;
        ipHintNode->setPosition({base.x - kIpFieldWidth * 0.5f + kIpFieldPadding + ipHintMesh->getWidth() * 0.5f,
                                 base.y + kIpHintOffsetY, 0.0f});
        localIpNode->setPosition({base.x - kIpFieldWidth * 0.5f + kIpFieldPadding + localIpMesh->getWidth() * 0.5f,
                                  base.y + kLocalIpOffsetY, 0.0f});
        netStatusNode->setPosition({base.x - kIpFieldWidth * 0.5f + kIpFieldPadding + netStatusMesh->getWidth() * 0.5f,
                                    base.y + kStatusOffsetY, 0.0f});
    }
} // Scenes
