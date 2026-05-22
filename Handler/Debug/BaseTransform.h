#ifndef SNAKE3_BASETRANSFORM_H
#define SNAKE3_BASETRANSFORM_H

#include "../../Manager/Camera.h"
#include "../../Renderer/Opengl/Model/Standard/MeshNode3D.h"

using namespace Model;
using namespace Manager;

namespace Debug {
    class BaseTransform {
    public:
        explicit BaseTransform(const shared_ptr<Camera> &camera);

        void addItem(const shared_ptr<MeshNode3D> &item);

        shared_ptr<MeshNode3D> findNextItem();

        shared_ptr<MeshNode3D> findFirstVisible();

        [[nodiscard]] glm::vec3 getItemWorldCenter() const;

        [[nodiscard]] glm::vec3 getWorldMax() const;

        [[nodiscard]] glm::vec3 getWorldMin() const;

        void deactivate() { enabled = false; }

        void deactivateAndRestore() {
            enabled = false;
            camera->setStickyPoint(cameraOriginalStickyPoint);
        }

        void activate();

        [[nodiscard]] bool isActiveItemVisible() const;

        [[nodiscard]] const vector<shared_ptr<MeshNode3D>>& getItems() const { return items; }
        [[nodiscard]] shared_ptr<MeshNode3D> getActiveItem() const { return activeItem; }

        // GUI direct selection - bypass keyboard cycling. Camera sticky point
        // se přepne na vybrané item; world bounds přepočte.
        void setActiveItem(const shared_ptr<MeshNode3D>& item);

    protected:
        void computeWorld();

        shared_ptr<Camera> camera = nullptr;
        vector<shared_ptr<MeshNode3D> > items;
        shared_ptr<MeshNode3D> activeItem = nullptr;
        shared_ptr<Transform> cameraOriginalStickyPoint = nullptr;
        bool enabled;
        glm::vec3 currentWorldCenter{};
        glm::vec3 worldMax{};
        glm::vec3 worldMin{};
    };
} // Debug

#endif //SNAKE3_BASETRANSFORM_H
