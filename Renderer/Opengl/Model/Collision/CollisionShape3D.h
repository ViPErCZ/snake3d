#ifndef SNAKE3_COLLISIONSHAPE3D_H
#define SNAKE3_COLLISIONSHAPE3D_H

#include "../../../../Physic/Shape.h"
#include "../Standard/MeshNode3D.h"
#include <memory>

using namespace Physic;
using namespace Model;

namespace CollisionShape {
    class CollisionShape3D : public MeshNode3D {
    public:
        CollisionShape3D(const shared_ptr<ContextState> &contextState,
                         const shared_ptr<ResourceManager> &resourceManager, const shared_ptr<Shape> &shape);

        void render(const shared_ptr<Camera> &camera, const glm::mat4 &projection, float dt,
                    const glm::mat4 &parentTransform, bool shadows) override;

        shared_ptr<Shape> getShape() const { return shape; }

    private:
        shared_ptr<Shape> shape;
    };
} // CollisionShape

#endif //SNAKE3_COLLISIONSHAPE3D_H
