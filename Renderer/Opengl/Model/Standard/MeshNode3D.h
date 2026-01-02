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
        explicit MeshNode3D(const shared_ptr<ContextState> &contextState,
            const shared_ptr<StandardMesh> &mesh, const shared_ptr<ResourceManager> &resourceManager);

        ~MeshNode3D() override = default;
        shared_ptr<StandardMesh> getMesh() const;
        void addNode(const std::shared_ptr<MeshNode3D> &node);

        virtual void render(const shared_ptr<Camera> &camera, const glm::mat4 &projection, float dt,
                    const glm::mat4 &parentTransform, bool shadows);

        virtual void update(float dt, uint64_t frameId);

        void renderShadows(const shared_ptr<Camera> &camera, const glm::mat4 &projection, float dt,
                           const glm::mat4 &parentTransform) const;

        [[nodiscard]] const vector<shared_ptr<MeshNode3D> > &getChildren() const;

        virtual void setDirectionalLight(const shared_ptr<DirectionalLight> &directional_light);

        void setTransformDetached(bool transform_detached, bool recursive = true);

        void make_unique();

        bool hasChildrenChangedSignal() const;

        void animationStart(const string &name, bool loop = true);

        void animationStop(const string &name) const;

        void animationPause(const string &name) const;

        void animationResume(const string &name) const;

        void disablePlanarReflection();

        [[nodiscard]] bool isIncludeInPlanarReflection() const;

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
        bool includePlanarReflection = true;
        int childrenChangedSignalCycles = 0;
        string animation;
        uint64_t lastUpdatedFrame;
    };
} // Model

#endif //SNAKE3_NODE3D_H
