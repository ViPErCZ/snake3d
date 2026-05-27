#ifndef SNAKE3_MESHNODE2D_H
#define SNAKE3_MESHNODE2D_H

#include <map>
#include <memory>

#include "BaseNode2D.h"
#include "../../../../../Tools/Vector3i.h"
#include "../../../../../Tools/Visibility.h"
#include "../../../../../Manager/Camera.h"
#include "../../../../../Manager/ResourceManager.h"
#include "../../../../../Tools/ContextState.h"

using namespace std;
using namespace Manager;

namespace Model {
    class MeshNode2D : public enable_shared_from_this<MeshNode2D>,
        public Node3D::Transform, public Node3D::Visibility, public Node3D::Vector3i {
    public:
        explicit MeshNode2D(const shared_ptr<Tools::ContextState> &contextState,
            const shared_ptr<BaseNode2D> &mesh, const shared_ptr<ResourceManager> &resourceManager);

        virtual void render(const shared_ptr<Camera> &camera, const glm::mat4 &ortho, float dt,
                    const glm::mat4 &parentTransform) const;

        virtual void update(float dt, uint64_t frameId);

        void addNode(const std::shared_ptr<MeshNode2D> &node, const std::string &name);

        void setTransformDetached(bool transform_detached, bool recursive = true);

        [[nodiscard]] const map<std::string, shared_ptr<MeshNode2D> > &getChildren() const;

    protected:
        shared_ptr<Tools::ContextState> contextState;
        shared_ptr<BaseNode2D> mesh;
        weak_ptr<MeshNode2D> parent;
        map<std::string, shared_ptr<MeshNode2D> > children;
        shared_ptr<ResourceManager> resourceManager;
        int depth = 0;
        bool transformDetached;
        uint64_t lastUpdatedFrame = 0;
    };
} // Model

#endif //SNAKE3_MESHNODE2D_H
