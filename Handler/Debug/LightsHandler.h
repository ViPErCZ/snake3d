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
        explicit LightsHandler(const shared_ptr<Tools::ContextState> &contextState,
            const shared_ptr<Manager::ResourceManager> &resourceManager, const shared_ptr<Manager::Camera> &camera);

        void onDefaultHandler() override;

        void onEventHandler(unsigned key, int scancode, int action, int mods, float deltaTime) override;

        void addItem(const shared_ptr<Light> &item);

        void active();

        void deactivate();

        shared_ptr<Light> findNextItem();

        shared_ptr<MeshNode2D> getFocusLabel() { return focusTextNode; }

        shared_ptr<MeshNode2D> getColorLabel() { return colorTextNode; }

        shared_ptr<MeshNode2D> getPositionLabel() { return positionTextNode; }

        shared_ptr<MeshNode2D> getDirectionLabel() { return directionTextNode; }

        shared_ptr<DirectionalLightNode3D> getDirLightNode() { return dirLightNode; }

        [[nodiscard]] shared_ptr<Light> getActiveItem() const { return activeItem; }
        [[nodiscard]] bool isEnabled() const { return enabled; }
        [[nodiscard]] const vector<shared_ptr<Light>>& getItems() const { return items; }

        // ImGui dropdown setter - cyklický next-item key pattern (findNextItem)
        // nestačí, GUI potřebuje přímý jump na zvolené světlo. Zachovává
        // camera sticky behavior tím, že kopíruje původní logiku z active().
        void setActiveItem(const shared_ptr<Light>& item);

    protected:
        [[nodiscard]] glm::vec3 getColorByFocus() const;
        void setColorByFocus(glm::vec3 color) const;
        shared_ptr<Manager::Camera> camera = nullptr;
        vector<shared_ptr<Light> > items;
        shared_ptr<Light> activeItem = nullptr;
        shared_ptr<Transform> cameraOriginalStickyPoint = nullptr;
        bool enabled;
        Focus focus = Focus::Ambient;
        shared_ptr<Material::LabelSettings> labelSettings;
        shared_ptr<LabelNode2D> focusText;
        shared_ptr<MeshNode2D> focusTextNode;
        shared_ptr<LabelNode2D> colorText;
        shared_ptr<MeshNode2D> colorTextNode;
        shared_ptr<LabelNode2D> positionText;
        shared_ptr<MeshNode2D> positionTextNode;
        shared_ptr<LabelNode2D> directionText;
        shared_ptr<MeshNode2D> directionTextNode;
        shared_ptr<DirectionalLightNode3D> dirLightNode;
    };
}

#endif //SNAKE3_LIGHTSHANDLER_H
