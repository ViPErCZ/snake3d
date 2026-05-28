#ifndef SNAKE3_MESHNODE2D_H
#define SNAKE3_MESHNODE2D_H

#include <map>
#include <memory>

#include <snake3d/Renderer/Opengl/Model/Standard/2D/BaseNode2D.h>
#include <snake3d/Tools/Vector3i.h>
#include <snake3d/Tools/Visibility.h>
#include <snake3d/Manager/Camera.h>
#include <snake3d/Manager/ResourceManager.h>
#include <snake3d/Tools/ContextState.h>

namespace Model {
    using std::shared_ptr;

    class MeshNode2D : public std::enable_shared_from_this<MeshNode2D>,
        public Node3D::Transform, public Node3D::Visibility, public Node3D::Vector3i {
    public:
        explicit MeshNode2D(const shared_ptr<Tools::ContextState> &contextState,
            const shared_ptr<BaseNode2D> &mesh, const shared_ptr<Manager::ResourceManager> &resourceManager);

        virtual void render(const shared_ptr<Manager::Camera> &camera, const glm::mat4 &ortho, float dt,
                    const glm::mat4 &parentTransform) const;

        virtual void update(float dt, uint64_t frameId);

        void addNode(const std::shared_ptr<MeshNode2D> &node, const std::string &name);

        void setTransformDetached(bool transform_detached, bool recursive = true);

        [[nodiscard]] const std::map<std::string, shared_ptr<MeshNode2D> > &getChildren() const;

    protected:
        shared_ptr<Tools::ContextState> contextState;
        shared_ptr<BaseNode2D> mesh;
        std::weak_ptr<MeshNode2D> parent;
        std::map<std::string, shared_ptr<MeshNode2D> > children;
        shared_ptr<Manager::ResourceManager> resourceManager;
        int depth = 0;
        bool transformDetached;
        uint64_t lastUpdatedFrame = 0;
    };
} // Model

#endif //SNAKE3_MESHNODE2D_H
