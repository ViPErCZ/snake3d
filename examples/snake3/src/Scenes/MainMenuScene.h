#ifndef SNAKE3_MAINMENUSCENE_H
#define SNAKE3_MAINMENUSCENE_H

#include <memory>
#include <string>
#include <glm/glm.hpp>

#include "OrbitSceneBase.h"
#include <snake3d/Manager/TextureManager.h>
#include <snake3d/Renderer/Opengl/Material/2D/LabelSettings.h>
#include <snake3d/Renderer/Opengl/Material/Particle/ParticleProcessMaterial.h>
#include <snake3d/Renderer/Opengl/Material/ShaderMaterial.h>
#include <snake3d/Renderer/Opengl/Material/Uniform/TimerUniform.h>
#include <snake3d/Renderer/Opengl/Model/Standard/2D/GPUParticle2D.h>
#include <snake3d/Renderer/Opengl/Model/Standard/2D/ImageNode2D.h>
#include <snake3d/Renderer/Opengl/Model/Standard/2D/LabelNode2D.h>
#include <snake3d/Renderer/Opengl/Model/Standard/2D/MeshNode2D.h>
#include <snake3d/Renderer/Opengl/Model/Standard/2D/QuadNode2D.h>

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
            const std::shared_ptr<Lights::DirectionalLight> &directionalLight,
            const std::vector<std::shared_ptr<Lights::SpotLight> > &spotLights,
            const std::vector<std::shared_ptr<Lights::PointLight> > &pointLights,
            const std::shared_ptr<Manager::RenderManager> &rendererManager,
            const std::shared_ptr<Manager::Camera> &camera, const glm::mat4 &projection,
            const std::shared_ptr<Manager::ResourceManager> &rm, int width, int height);

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
            std::shared_ptr<Model::MeshNode2D> backgroundNode;
            std::shared_ptr<Model::MeshNode2D> labelNode;
            std::shared_ptr<Model::LabelNode2D> labelMesh;
            std::shared_ptr<Material::ShaderMaterial> backgroundMaterial;
            std::shared_ptr<Material::ShaderMaterial> textMaterial;
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

        std::shared_ptr<Material::Font> buttonFont;
        std::shared_ptr<Material::LabelSettings> buttonSettings;
        std::shared_ptr<Uniform::TimerUniform> titleTimer;
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
        std::shared_ptr<Material::Font> netInfoFont;
        std::shared_ptr<Material::LabelSettings> netInfoSettings;
        std::shared_ptr<Model::LabelNode2D> localIpMesh;
        std::shared_ptr<Model::MeshNode2D> localIpNode;
        std::shared_ptr<Material::ShaderMaterial> localIpMaterial;
        std::shared_ptr<Model::LabelNode2D> ipHintMesh;
        std::shared_ptr<Model::MeshNode2D> ipHintNode;
        std::shared_ptr<Material::ShaderMaterial> ipHintMaterial;
        std::shared_ptr<Model::LabelNode2D> netStatusMesh;
        std::shared_ptr<Model::MeshNode2D> netStatusNode;
        std::shared_ptr<Material::ShaderMaterial> netStatusMaterial;
        std::string localIpLabel = "IP: -";
        std::string networkStatus = "Status: idle";
        NetworkSessionState networkSessionState = NetworkSessionState::Idle;
        glm::vec2 networkInfoBase{0.0f, 0.0f};
        std::shared_ptr<Model::MeshNode2D> backdropNode;
        std::shared_ptr<Model::MeshNode2D> titleNode;

        std::shared_ptr<Model::ImageNode2D> cursorMesh;
        std::shared_ptr<Model::MeshNode2D> cursorNode;
        std::shared_ptr<Material::ParticleProcessMaterial> cursorTrailMaterial;
        std::shared_ptr<Model::GPUParticle2D> cursorTrail;
        std::shared_ptr<Manager::TextureManager> cursorTexture;
        glm::vec2 cursorSize{0.0f, 0.0f};
        glm::vec2 cursorHotspot{0.0f, 0.0f};
        glm::vec2 lastCursorScreenPos{0.0f, 0.0f};
        bool cursorInitialized = false;
        bool hasCursorLastPos = false;
    };
} // Scenes

#endif //SNAKE3_MAINMENUSCENE_H
