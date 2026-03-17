#ifndef SNAKE3_MAINMENUSCENE_H
#define SNAKE3_MAINMENUSCENE_H

#include <memory>
#include <string>
#include <glm/glm.hpp>

#include "OrbitSceneBase.h"
#include "../Renderer/Opengl/Material/2D/LabelSettings.h"
#include "../Renderer/Opengl/Material/ShaderMaterial.h"
#include "../Renderer/Opengl/Material/Uniform/TimerUniform.h"
#include "../Renderer/Opengl/Model/Standard/2D/MeshNode2D.h"
#include "../Renderer/Opengl/Model/Standard/2D/QuadNode2D.h"
#include "../Renderer/Opengl/Model/Standard/2D/ImageNode2D.h"

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
            Quit
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

        void setCursorPosition(const glm::vec2 &position);
        MenuAction handleMouseButton(int button, int action);
        void setPrimaryAction(PrimaryAction action);
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
        void initTitle(const glm::vec2 &center);
        void initButtons(const glm::vec2 &center);
        MenuButton buildButton(const std::string &text, const glm::vec2 &center, const glm::vec2 &size);
        void updateHoverState();
        void applyButtonStyle(const MenuButton &button, bool hovered);
        [[nodiscard]] bool hitTest(const MenuButton &button) const;

        glm::vec2 cursorScreenPos{ -1.0f, -1.0f };
        bool cursorValid = false;

        shared_ptr<Font> buttonFont;
        shared_ptr<LabelSettings> buttonSettings;
        shared_ptr<TimerUniform> titleTimer;
        MenuButton startButton;
        MenuButton quitButton;
        PrimaryAction primaryAction = PrimaryAction::Start;
        glm::vec2 viewportCenter{0.0f, 0.0f};
    };
} // Scenes

#endif //SNAKE3_MAINMENUSCENE_H
