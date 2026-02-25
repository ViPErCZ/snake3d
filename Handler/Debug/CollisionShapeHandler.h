#ifndef SNAKE3_COLLISIONSHAPEHANDLER_H
#define SNAKE3_COLLISIONSHAPEHANDLER_H

#include "../BaseKeydownHandle.h"
#include "../../Manager/Camera.h"
#include "../../Renderer/Opengl/Model/Collision/CollisionShape3D.h"

using namespace Manager;
using namespace CollisionShape;

// Collision shape size manipulation
// ==============================================
namespace Handler::Debug {
    class CollisionShapeHandler : public BaseKeydownHandle {
    public:
        explicit CollisionShapeHandler(const shared_ptr<Camera> &camera);

        void onDefaultHandler() override;

        void onEventHandler(unsigned key, int scancode, int action, int mods, float deltaTime) override;

        void addItem(const shared_ptr<CollisionShape3D> &item);
        
        void active();
        
        void deactivate() { enabled = false; }
        
        shared_ptr<CollisionShape3D> findNextItem();

        shared_ptr<CollisionShape3D> findFirstVisible();

    protected:
        void computeWorld();
        shared_ptr<Camera> camera = nullptr;
        vector<shared_ptr<CollisionShape3D> > items;
        shared_ptr<CollisionShape3D> activeItem = nullptr;
        shared_ptr<Transform> cameraOriginalStickyPoint = nullptr;
        bool enabled;
        glm::vec3 currentWorldCenter{};
        glm::vec3 worldMax{};
        glm::vec3 worldMin{};
    };
} // Debug
// Handler

#endif //SNAKE3_COLLISIONSHAPEHANDLER_H
