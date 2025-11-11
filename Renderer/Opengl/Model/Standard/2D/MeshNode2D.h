#ifndef SNAKE3_MESHNODE2D_H
#define SNAKE3_MESHNODE2D_H

#include <memory>

#include "BaseNode2D.h"
#include "../../../../../ItemsDto/Visibility.h"
#include "../../../../../Manager/Camera.h"
#include "../../../../../Manager/ResourceManager.h"

using namespace std;
using namespace Manager;

namespace Model {
    class MeshNode2D : public enable_shared_from_this<MeshNode2D>, public Transform, public Visibility {
    public:
        explicit MeshNode2D(const shared_ptr<BaseNode2D> &mesh, const shared_ptr<ResourceManager> &resourceManager);

        void render(const shared_ptr<Camera> &camera, const glm::mat4 &ortho, float dt,
                    const glm::mat4 &parentTransform) const;

        virtual void update(float dt);

    protected:
        shared_ptr<BaseNode2D> mesh;
        weak_ptr<MeshNode2D> parent;
        vector<shared_ptr<MeshNode2D> > children;
        shared_ptr<ResourceManager> resourceManager;
        int depth = 0;
        bool transformDetached;
    };
} // Model

#endif //SNAKE3_MESHNODE2D_H
