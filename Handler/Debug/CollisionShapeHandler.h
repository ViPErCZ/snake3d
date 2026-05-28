#ifndef SNAKE3_COLLISIONSHAPEHANDLER_H
#define SNAKE3_COLLISIONSHAPEHANDLER_H

#include "../BaseKeydownHandle.h"
#include "../../Manager/Camera.h"
#include "../../Renderer/Opengl/Model/Collision/CollisionShape3D.h"

// Collision shape size manipulation
// ==============================================
namespace Handler::Debug {
    using CollisionShape::CollisionShape3D;

    class CollisionShapeHandler : public BaseKeydownHandle {
    public:
        explicit CollisionShapeHandler(const std::shared_ptr<Manager::Camera> &camera);

        void onDefaultHandler() override;

        void onEventHandler(unsigned key, int scancode, int action, int mods, float deltaTime) override;

        void addItem(const std::shared_ptr<CollisionShape3D> &item);
        
        void active();
        
        void deactivate() { enabled = false; }
        
        std::shared_ptr<CollisionShape3D> findNextItem();

        std::shared_ptr<CollisionShape3D> findFirstVisible();

        // Globální toggle viditelnosti všech collision shapes (ImGui overlay).
        // Per-shape F8 cycling stále funguje paralelně - po setAllVisible(false)
        // může uživatel přepnout F8 mode a opět individuálně rozsvítit shape.
        void setAllVisible(bool visible);
        [[nodiscard]] bool isAllVisible() const;

        [[nodiscard]] const std::vector<std::shared_ptr<CollisionShape3D>>& getItems() const { return items; }
        [[nodiscard]] std::shared_ptr<CollisionShape3D> getActiveItem() const { return activeItem; }
        void setActiveItem(const std::shared_ptr<CollisionShape3D>& item);

    protected:
        std::shared_ptr<Manager::Camera> camera = nullptr;
        std::vector<std::shared_ptr<CollisionShape3D> > items;
        std::shared_ptr<CollisionShape3D> activeItem = nullptr;
        std::shared_ptr<Node3D::Transform> cameraOriginalStickyPoint = nullptr;
        bool enabled;
    };
} // Debug
// Handler

#endif //SNAKE3_COLLISIONSHAPEHANDLER_H
