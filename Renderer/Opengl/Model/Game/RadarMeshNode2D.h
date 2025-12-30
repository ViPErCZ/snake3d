#ifndef SNAKE3_RADARMESHNODE2D_H
#define SNAKE3_RADARMESHNODE2D_H

#include <map>

#include "RadarItem.h"
#include "../Standard/MeshNode3D.h"
#include "../Standard/2D/MeshNode2D.h"
#include "../Standard/2D/QuadNode2D.h"

namespace Model {
    class RadarMeshNode2D final : public MeshNode2D {
    public:
        RadarMeshNode2D(const shared_ptr<QuadNode2D> &mesh,
            const shared_ptr<ResourceManager> &resourceManager);

        void render(const shared_ptr<Camera> &camera, const glm::mat4 &ortho, float dt,
            const glm::mat4 &parentTransform) const override;

        void update(float dt, uint64_t frameId) override;

        void addItem(const shared_ptr<MeshNode3D> &item, const glm::vec3 &color, const std::string &name);

        void hideItems() const;

        void showItems() const;

        void hideItem(const std::string &name) const;

        void showItem(const std::string &name) const;

    private:
        map<std::string, shared_ptr<RadarItem> > items;
    };
} // Model

#endif //SNAKE3_RADARMESHNODE2D_H