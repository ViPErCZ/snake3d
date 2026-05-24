#ifndef SNAKE3_MAINMENUSCENE_H
#define SNAKE3_MAINMENUSCENE_H

#include <memory>
#include <string>
#include <glm/glm.hpp>

#include "OrbitSceneBase.h"
#include "Manager/TextureManager.h"
#include "Renderer/Opengl/Material/2D/LabelSettings.h"
#include "Renderer/Opengl/Material/Particle/ParticleProcessMaterial.h"
#include "Renderer/Opengl/Material/ShaderMaterial.h"
#include "Renderer/Opengl/Material/Uniform/TimerUniform.h"
#include "Renderer/Opengl/Model/Standard/2D/GPUParticle2D.h"
#include "Renderer/Opengl/Model/Standard/2D/ImageNode2D.h"
#include "Renderer/Opengl/Model/Standard/2D/LabelNode2D.h"
#include "Renderer/Opengl/Model/Standard/2D/MeshNode2D.h"
#include "Renderer/Opengl/Model/Standard/2D/QuadNode2D.h"

using namespace Material;
using namespace Model;
using namespace Uniform;

namespace Scenes {
    class MainMenuScene final : public OrbitSceneBase {
    public:
        enum class PrimaryAction {
            Start,
            Resume
        };

        enum class MenuAction {
            None,
            Start,
            Resume,
            NewGame,
            Disconnect,
            Host,
            Join,
            Quit
        };

        enum class MenuView {
            Main,
            Network
        };

        enum class NetworkSessionState {
            Idle,
            Hosting,
            Client
        };

        MainMenuScene(
            const shared_ptr<DirectionalLight> &directionalLight,
            const vector<shared_ptr<SpotLight> > &spotLights,
            const vector<shared_ptr<PointLight> > &pointLights,
            const shared_ptr<RenderManager> &rendererManager,
            const shared_ptr<Camera> &camera, const glm::mat4 &projection,
            const shared_ptr<ResourceManager> &rm, int width, int height);

        void init(int priority) override;
        void update() override;
        void resize(int width, int height, const glm::mat4 &projection) override;

        void setCursorPosition(const glm::vec2 &position);
        MenuAction handleMouseButton(int button, int action);
        void setPrimaryAction(PrimaryAction action);
        void handleKeyInput(int key, int action);
        void setLocalIp(const std::string &ip);
        void setNetworkStatus(std::string status);
        void setNetworkSessionState(NetworkSessionState state);
        void setMenuView(MenuView view);
        [[nodiscard]] const std::string &getJoinIp() const { return joinIp; }
        [[nodiscard]] bool consumeJoinRequest();
        [[nodiscard]] PrimaryAction getPrimaryAction() const { return primaryAction; }

    private:
        struct MenuButton {
            glm::vec2 center{};
            glm::vec2 size{};
            glm::vec2 labelOffset{};
            shared_ptr<MeshNode2D> backgroundNode;
            shared_ptr<MeshNode2D> labelNode;
            shared_ptr<LabelNode2D> labelMesh;
            shared_ptr<ShaderMaterial> backgroundMaterial;
            shared_ptr<ShaderMaterial> textMaterial;
            bool hovered = false;
        };

        void initBackdrop();
        void initCursor();
        void updateCursor();
        void initTitle(const glm::vec2 &center);
        void initButtons(const glm::vec2 &center);
        MenuButton buildButton(const std::string &text, const glm::vec2 &center, const glm::vec2 &size);
        MenuButton buildIpField(const glm::vec2 &center, const glm::vec2 &size);
        void updateHoverState();
        void applyButtonStyle(const MenuButton &button, bool hovered);
        [[nodiscard]] bool hitTest(const MenuButton &button) const;
        void updateIpLabel();
        void updateNetworkLabels();
        void updateLayout();
        void setButtonVisible(const MenuButton &button, bool visible) const;
        void setButtonText(const MenuButton &button, const std::string &text) const;
        void layoutButton(MenuButton &button) const;

        glm::vec2 cursorScreenPos{ -1.0f, -1.0f };
        bool cursorValid = false;

        shared_ptr<Font> buttonFont;
        shared_ptr<LabelSettings> buttonSettings;
        shared_ptr<TimerUniform> titleTimer;
        MenuButton startButton;
        MenuButton networkButton;
        MenuButton newGameButton;
        MenuButton hostButton;
        MenuButton joinButton;
        MenuButton ipField;
        MenuButton backButton;
        MenuButton quitButton;
        PrimaryAction primaryAction = PrimaryAction::Start;
        MenuView menuView = MenuView::Main;
        glm::vec2 viewportCenter{0.0f, 0.0f};
        std::string joinIp = "127.0.0.1";
        bool ipInputActive = false;
        bool caretVisible = false;
        int caretIndex = 0;
        std::string lastIpRender;
        bool joinRequested = false;
        shared_ptr<Font> netInfoFont;
        shared_ptr<LabelSettings> netInfoSettings;
        shared_ptr<LabelNode2D> localIpMesh;
        shared_ptr<MeshNode2D> localIpNode;
        shared_ptr<ShaderMaterial> localIpMaterial;
        shared_ptr<LabelNode2D> ipHintMesh;
        shared_ptr<MeshNode2D> ipHintNode;
        shared_ptr<ShaderMaterial> ipHintMaterial;
        shared_ptr<LabelNode2D> netStatusMesh;
        shared_ptr<MeshNode2D> netStatusNode;
        shared_ptr<ShaderMaterial> netStatusMaterial;
        std::string localIpLabel = "IP: -";
        std::string networkStatus = "Status: idle";
        NetworkSessionState networkSessionState = NetworkSessionState::Idle;
        glm::vec2 networkInfoBase{0.0f, 0.0f};
        shared_ptr<MeshNode2D> backdropNode;
        shared_ptr<MeshNode2D> titleNode;

        shared_ptr<ImageNode2D> cursorMesh;
        shared_ptr<MeshNode2D> cursorNode;
        shared_ptr<ParticleProcessMaterial> cursorTrailMaterial;
        shared_ptr<GPUParticle2D> cursorTrail;
        shared_ptr<TextureManager> cursorTexture;
        glm::vec2 cursorSize{0.0f, 0.0f};
        glm::vec2 cursorHotspot{0.0f, 0.0f};
        glm::vec2 lastCursorScreenPos{0.0f, 0.0f};
        bool cursorInitialized = false;
        bool hasCursorLastPos = false;
    };
} // Scenes

#endif //SNAKE3_MAINMENUSCENE_H
