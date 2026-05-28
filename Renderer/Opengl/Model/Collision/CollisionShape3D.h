#ifndef SNAKE3_COLLISIONSHAPE3D_H
#define SNAKE3_COLLISIONSHAPE3D_H

#include "../../../../Physic/Shape.h"
#include "../Standard/MeshNode3D.h"
#include <memory>
#include <set>

namespace CollisionShape {
    class CollisionShape3D : public Model::MeshNode3D {
    public:
        CollisionShape3D(const std::shared_ptr<Tools::ContextState> &contextState,
                         const std::shared_ptr<Manager::ResourceManager> &resourceManager, const std::shared_ptr<Physic::Shape> &shape);

        void render(const std::shared_ptr<Manager::Camera> &camera, const glm::mat4 &projection, float dt,
                    const glm::mat4 &parentTransform, bool shadows) override;

        [[nodiscard]] bool isCollisionShapeNode() const override { return true; }

        std::shared_ptr<Physic::Shape> getShape() const { return shape; }

        std::shared_ptr<Model::StandardMesh> getMesh() const override;

        void clearCollisions() { collidingBodies.clear(); }

        void addCollidingBody(const std::shared_ptr<Model::MeshNode3D> &body) {
            collidingBodies.insert(body);
        }

        bool isCollidingWith(const std::shared_ptr<Model::MeshNode3D> &body) const {
            return collidingBodies.contains(body);
        }

        const std::set<std::shared_ptr<Model::MeshNode3D> > &getCollidingBodies() const {
            return collidingBodies;
        }

        bool hasCollisions() const { return !collidingBodies.empty(); }

        void setCollisionLayer(const uint32_t layer) { collisionLayer = layer; }
        [[nodiscard]] uint32_t getCollisionLayer() const { return collisionLayer; }

        void setCollisionMask(const uint32_t mask) { collisionMask = mask; }
        [[nodiscard]] uint32_t getCollisionMask() const { return collisionMask; }

        static bool shouldCollide(const uint32_t layerA, const uint32_t maskA, const uint32_t layerB, const uint32_t maskB) {
            return (maskA & layerB) != 0 && (maskB & layerA) != 0;
        }

    private:
        std::shared_ptr<Physic::Shape> shape;
        std::set<std::shared_ptr<Model::MeshNode3D> > collidingBodies;
        uint32_t collisionLayer = 1;
        uint32_t collisionMask = 1;
    };
} // CollisionShape

#endif //SNAKE3_COLLISIONSHAPE3D_H
