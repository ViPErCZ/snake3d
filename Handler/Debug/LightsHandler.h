#ifndef SNAKE3_LIGHTSHANDLER_H
#define SNAKE3_LIGHTSHANDLER_H

#include <vector>
#include "../BaseKeydownHandle.h"
#include "../../Lights/Light.h"
#include "../../Manager/Camera.h"
#include "../../Manager/ResourceManager.h"
#include "../../Renderer/Opengl/Material/2D/LabelSettings.h"
#include "../../Renderer/Opengl/Model/Debug/DirectionalLightNode3D.h"
#include "../../Renderer/Opengl/Model/Standard/2D/LabelNode2D.h"
#include "../../Renderer/Opengl/Model/Standard/2D/MeshNode2D.h"

namespace Handler::Debug {
    using Lights::Light;
    using Model::LabelNode2D;
    using Model::MeshNode2D;
    using Model::DirectionalLightNode3D;

    enum class Focus {
        Ambient, Diffuse, Specular, Direction, Position,
    };
    class LightsHandler : public BaseKeydownHandle {
    public:
        explicit LightsHandler(const std::shared_ptr<Tools::ContextState> &contextState,
            const std::shared_ptr<Manager::ResourceManager> &resourceManager, const std::shared_ptr<Manager::Camera> &camera);

        void onDefaultHandler() override;

        void onEventHandler(unsigned key, int scancode, int action, int mods, float deltaTime) override;

        void addItem(const std::shared_ptr<Light> &item);

        void active();

        void deactivate();

        std::shared_ptr<Light> findNextItem();

        std::shared_ptr<MeshNode2D> getFocusLabel() { return focusTextNode; }

        std::shared_ptr<MeshNode2D> getColorLabel() { return colorTextNode; }

        std::shared_ptr<MeshNode2D> getPositionLabel() { return positionTextNode; }

        std::shared_ptr<MeshNode2D> getDirectionLabel() { return directionTextNode; }

        std::shared_ptr<DirectionalLightNode3D> getDirLightNode() { return dirLightNode; }

        [[nodiscard]] std::shared_ptr<Light> getActiveItem() const { return activeItem; }
        [[nodiscard]] bool isEnabled() const { return enabled; }
        [[nodiscard]] const std::vector<std::shared_ptr<Light>>& getItems() const { return items; }

        // ImGui dropdown setter - cyklický next-item key pattern (findNextItem)
        // nestačí, GUI potřebuje přímý jump na zvolené světlo. Zachovává
        // camera sticky behavior tím, že kopíruje původní logiku z active().
        void setActiveItem(const std::shared_ptr<Light>& item);

    protected:
        [[nodiscard]] glm::vec3 getColorByFocus() const;
        void setColorByFocus(glm::vec3 color) const;
        std::shared_ptr<Manager::Camera> camera = nullptr;
        std::vector<std::shared_ptr<Light> > items;
        std::shared_ptr<Light> activeItem = nullptr;
        std::shared_ptr<Node3D::Transform> cameraOriginalStickyPoint = nullptr;
        bool enabled;
        Focus focus = Focus::Ambient;
        std::shared_ptr<Material::LabelSettings> labelSettings;
        std::shared_ptr<LabelNode2D> focusText;
        std::shared_ptr<MeshNode2D> focusTextNode;
        std::shared_ptr<LabelNode2D> colorText;
        std::shared_ptr<MeshNode2D> colorTextNode;
        std::shared_ptr<LabelNode2D> positionText;
        std::shared_ptr<MeshNode2D> positionTextNode;
        std::shared_ptr<LabelNode2D> directionText;
        std::shared_ptr<MeshNode2D> directionTextNode;
        std::shared_ptr<DirectionalLightNode3D> dirLightNode;
    };
}

#endif //SNAKE3_LIGHTSHANDLER_H
