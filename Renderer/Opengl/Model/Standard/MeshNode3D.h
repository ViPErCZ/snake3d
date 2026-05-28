#ifndef SNAKE3_NODE3D_H
#define SNAKE3_NODE3D_H

#include <memory>

#include "StandardMesh.h"
#include <snake3d/Tools/Named.h>
#include <snake3d/Tools/Vector3i.h>
#include <snake3d/Tools/Tagged.h>
#include <snake3d/Tools/Visibility.h>
#include "../../../../Manager/ResourceManager.h"
#include <snake3d/Lights/DirectionalLight.h>
#include <snake3d/Lights/PointLight.h>
#include <snake3d/Lights/SpotLight.h>
#include <snake3d/Tools/ContextState.h>

namespace CollisionShape {
    class CollisionShape3D;
}

namespace Model {
    class MeshNode3D : public std::enable_shared_from_this<MeshNode3D>,
        public Node3D::Named, public Node3D::Tagged, public Node3D::Transform,
        public Node3D::Visibility, public Node3D::Vector3i {
    public:
        explicit MeshNode3D(const std::shared_ptr<Tools::ContextState> &contextState,
            const std::shared_ptr<StandardMesh> &mesh, const std::shared_ptr<Manager::ResourceManager> &resourceManager);

        ~MeshNode3D() override;

        [[nodiscard]] virtual std::shared_ptr<StandardMesh> getMesh() const;

        void addNode(const std::shared_ptr<MeshNode3D> &node);

        virtual void render(const std::shared_ptr<Manager::Camera> &camera, const glm::mat4 &projection, float dt,
                    const glm::mat4 &parentTransform, bool shadows);

        virtual void update(float dt, uint64_t frameId);

        virtual void renderShadows(const std::shared_ptr<Manager::Camera> &camera, const glm::mat4 &projection, float dt,
                           const glm::mat4 &parentTransform) const;

        [[nodiscard]] const std::vector<std::shared_ptr<MeshNode3D> > &getChildren() const;

        std::shared_ptr<MeshNode3D> getParent() const { return parent.lock(); }
        // Public setter pro případy, kdy node není přidaný přes addNode (e.g.
        // CollisionShape3D pod SnakeMeshNode3D::setCollisionShape - žije v
        // collisionShapes, ne children, ale parent chain je potřeba pro
        // inspector/physics lookup via shape->getParent()).
        void setParent(const std::shared_ptr<MeshNode3D> &p) { parent = p; }

        [[nodiscard]] const std::vector<std::shared_ptr<CollisionShape::CollisionShape3D> > &getCollisionShapes() const;

        virtual void setDirectionalLight(const std::shared_ptr<Lights::DirectionalLight> &directional_light);

        virtual void setSpotLights(const std::vector<std::shared_ptr<Lights::SpotLight> > &spot_light);

        virtual void setPointLights(const std::vector<std::shared_ptr<Lights::PointLight> > &point_light);

        void setTransformDetached(bool transform_detached, bool recursive = true);

        void make_unique();

        bool hasChildrenChangedSignal() const;

        void animationStart(const std::string &name, bool loop = true);

        void animationStop(const std::string &name) const;

        void animationPause(const std::string &name) const;

        void animationResume(const std::string &name) const;

        void disablePlanarReflection();

        [[nodiscard]] bool isIncludeInPlanarReflection() const;

        std::string getAnimation() { return animation; }

        [[nodiscard]] virtual bool isCollisionShapeNode() const { return false; }

        virtual void computeWorldMatrix(const glm::mat4 &parentTransform);

        [[nodiscard]] glm::mat4 getWorldMatrix() const override { return worldMatrixCache; }

    protected:
        std::shared_ptr<MeshNode3D> deepCopy() const;

        std::shared_ptr<Tools::ContextState> contextState;
        std::shared_ptr<StandardMesh> mesh;
        std::weak_ptr<MeshNode3D> parent;
        std::vector<std::shared_ptr<MeshNode3D> > children;
        std::vector<std::shared_ptr<CollisionShape::CollisionShape3D> > collisionShapes;
        std::shared_ptr<Manager::ResourceManager> resourceManager;
        std::shared_ptr<Lights::DirectionalLight> directionalLight;
        std::vector<std::shared_ptr<Lights::SpotLight> > spotLights;
        std::vector<std::shared_ptr<Lights::PointLight> > pointLights;
        int depth = 0;
        bool transformDetached;
        bool childrenChangedSignal;
        bool includePlanarReflection = true;
        int childrenChangedSignalCycles = 0;
        std::string animation;
        uint64_t lastUpdatedFrame;
        glm::mat4 worldMatrixCache;
        // Snapshot rodičovské matice z minulého frame - když přijde stejná +
        // self není dirty, computeWorldMatrix subtree skip (nutí children
        // recompute pokud parent move, ale floor cells & static props
        // 0 work per frame po prvním compute).
        // sentinel mat4(0): zaručí first-frame recompute (žádný legitimní
        // parent transform není all-zeros).
        glm::mat4 lastParentMatrix{0.0f};
    };
} // Model

#endif //SNAKE3_NODE3D_H
