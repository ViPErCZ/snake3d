#include "CoinMeshNode3D.h"

namespace Model {
    void CoinMeshNode3D::update(const float dt) {
        MeshNode3D::update(dt);

        const float rotate = mesh->getBaseItem()->getRotationY();

        const double now = glfwGetTime();
        float angle = rotate;
        if (now > lastTime + 0.005) {
            angle++;
            lastTime = now;
        }

        mesh->getBaseItem()->setRotationX(90);
        mesh->getBaseItem()->setRotationY(angle);
    }
} // Model