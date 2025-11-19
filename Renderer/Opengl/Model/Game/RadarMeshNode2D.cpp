#include "RadarMeshNode2D.h"

namespace Model {
    RadarMeshNode2D::RadarMeshNode2D(const shared_ptr<QuadNode2D> &mesh,
                                     const shared_ptr<ResourceManager> &resourceManager) : MeshNode2D(
        mesh, resourceManager) {
    }

    void RadarMeshNode2D::render(const shared_ptr<Camera> &camera, const glm::mat4 &ortho, float dt,
        const glm::mat4 &parentTransform) const {
        glEnable(GL_BLEND);
        //glBlendFunc(GL_SRC_COLOR, GL_ONE_MINUS_DST_COLOR);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        MeshNode2D::render(camera, ortho, dt, parentTransform);
        glDisable(GL_BLEND);
    }

    void RadarMeshNode2D::addItem(const shared_ptr<Transform> &item) {
        items.push_back(item);
        // vytvorit quad node s nejakou barvou a predat transform souradnice
        // udelat scale
        // v update smycce aktualizovat souradnice jednotlivych items s nodama?
    }
} // Model
