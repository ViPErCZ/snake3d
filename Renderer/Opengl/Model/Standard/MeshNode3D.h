#ifndef SNAKE3_NODE3D_H
#define SNAKE3_NODE3D_H

#include <memory>
#include "StandardMesh.h"
#include "../../../../Manager/ResourceManager.h"

namespace Model {
    class MeshNode3D : public enable_shared_from_this<MeshNode3D>, public Transform, public Vector3i {
    public:
        explicit MeshNode3D(const shared_ptr<StandardMesh> &mesh, const shared_ptr<ResourceManager> &resourceManager);

        ~MeshNode3D() override = default;

        void addNode(const std::shared_ptr<MeshNode3D> &node);

        void render(const shared_ptr<Camera> &camera, const glm::mat4 &projection, float dt,
                    const glm::mat4 &parentTransform) const;

        void update(float dt) const;

        void renderShadows(const shared_ptr<Camera> &camera, const glm::mat4 &projection, float dt,
                           const glm::mat4 &parentTransform) const;

        shared_ptr<BaseItem> getBaseItem() const;

        [[nodiscard]] glm::mat4 getModelMatrix() const override;

        [[nodiscard]] const vector<shared_ptr<MeshNode3D> > &getChildren() const;

        virtual void setDirectionalLight(const shared_ptr<DirectionalLight> &directional_light);

        void make_unique();

    protected:
        std::shared_ptr<MeshNode3D> deepCopy() const;

        shared_ptr<StandardMesh> mesh;
        weak_ptr<MeshNode3D> parent;
        vector<shared_ptr<MeshNode3D> > children;
        shared_ptr<ResourceManager> resourceManager;
        shared_ptr<DirectionalLight> directionalLight;
        int depth = 0;
        bool transformDetached;
    };
} // Model

#endif //SNAKE3_NODE3D_H
