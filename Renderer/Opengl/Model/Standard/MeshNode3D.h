#ifndef SNAKE3_NODE3D_H
#define SNAKE3_NODE3D_H

#include <memory>
#include "StandardMesh.h"
#include "../../../../ItemsDto/Visibility.h"
#include "../../../../Manager/ResourceManager.h"
#include "../../../../Tools/ContextState.h"

using namespace Tools;

namespace Model {
    class MeshNode3D : public enable_shared_from_this<MeshNode3D>, public Transform, public Visibility, public Vector3i {
    public:
        explicit MeshNode3D(const shared_ptr<StandardMesh> &mesh, const shared_ptr<ResourceManager> &resourceManager);

        ~MeshNode3D() override = default;

        void addNode(const std::shared_ptr<MeshNode3D> &node);

        virtual void render(const shared_ptr<Camera> &camera, const glm::mat4 &projection, float dt,
                    const glm::mat4 &parentTransform, bool shadows);

        virtual void update(float dt);

        void renderShadows(const shared_ptr<Camera> &camera, const glm::mat4 &projection, float dt,
                           const glm::mat4 &parentTransform) const;

        [[nodiscard]] const vector<shared_ptr<MeshNode3D> > &getChildren() const;

        virtual void setDirectionalLight(const shared_ptr<DirectionalLight> &directional_light);

        void setTransformDetached(bool transform_detached, bool recursive = true);

        void make_unique();

        bool hasChildrenChangedSignal() const;

        void setBlending(Blending blending);

    protected:
        shared_ptr<MeshNode3D> deepCopy() const;

        shared_ptr<ContextState> contextState;
        shared_ptr<StandardMesh> mesh;
        weak_ptr<MeshNode3D> parent;
        vector<shared_ptr<MeshNode3D> > children;
        shared_ptr<ResourceManager> resourceManager;
        shared_ptr<DirectionalLight> directionalLight;
        int depth = 0;
        bool transformDetached;
        bool childrenChangedSignal;
        int childrenChangedSignalCycles = 0;
    };
} // Model

#endif //SNAKE3_NODE3D_H
