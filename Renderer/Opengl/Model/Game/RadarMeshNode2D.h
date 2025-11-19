#ifndef SNAKE3_RADARMESHNODE2D_H
#define SNAKE3_RADARMESHNODE2D_H

#include "../Standard/2D/MeshNode2D.h"
#include "../Standard/2D/QuadNode2D.h"

namespace Model {
    class RadarMeshNode2D final : public MeshNode2D {
    public:
        RadarMeshNode2D(const shared_ptr<QuadNode2D> &mesh,
            const shared_ptr<ResourceManager> &resourceManager);

        void render(const shared_ptr<Camera> &camera, const glm::mat4 &ortho, float dt,
            const glm::mat4 &parentTransform) const override;

        void addItem(const shared_ptr<Transform> &item);

    private:
        vector<shared_ptr<Transform> > items;
    };
} // Model

#endif //SNAKE3_RADARMESHNODE2D_H