#ifndef SNAKE3_MESHNODE2D_H
#define SNAKE3_MESHNODE2D_H

#include <map>
#include <memory>

#include "BaseNode2D.h"
#include "../../../../../ItemsDto/Visibility.h"
#include "../../../../../Manager/Camera.h"
#include "../../../../../Manager/ResourceManager.h"

using namespace std;
using namespace Manager;

namespace Model {
    class MeshNode2D : public enable_shared_from_this<MeshNode2D>, public Transform, public Visibility, public Vector3i {
    public:
        explicit MeshNode2D(const shared_ptr<BaseNode2D> &mesh, const shared_ptr<ResourceManager> &resourceManager);

        virtual void render(const shared_ptr<Camera> &camera, const glm::mat4 &ortho, float dt,
                    const glm::mat4 &parentTransform) const;

        virtual void update(float dt, uint64_t frameId);

        void addNode(const std::shared_ptr<MeshNode2D> &node, const std::string &name);

        void setTransformDetached(bool transform_detached, bool recursive = true);

        [[nodiscard]] const map<std::string, shared_ptr<MeshNode2D> > &getChildren() const;

    protected:
        shared_ptr<BaseNode2D> mesh;
        weak_ptr<MeshNode2D> parent;
        map<std::string, shared_ptr<MeshNode2D> > children;
        shared_ptr<ResourceManager> resourceManager;
        int depth = 0;
        bool transformDetached;
    };
} // Model

#endif //SNAKE3_MESHNODE2D_H
