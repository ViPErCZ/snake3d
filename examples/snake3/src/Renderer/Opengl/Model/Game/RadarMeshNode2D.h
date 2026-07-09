#ifndef SNAKE3_RADARMESHNODE2D_H
#define SNAKE3_RADARMESHNODE2D_H

#include <map>

#include "RadarItem.h"
#include <snake3d/Renderer/Opengl/Model/Standard/MeshNode3D.h>
#include <snake3d/Renderer/Opengl/Model/Standard/2D/MeshNode2D.h>
#include <snake3d/Renderer/Opengl/Model/Standard/2D/QuadNode2D.h>

namespace Model {
    class RadarMeshNode2D final : public MeshNode2D {
    public:
        RadarMeshNode2D(const std::shared_ptr<Tools::ContextState> &contextState, const std::shared_ptr<QuadNode2D> &mesh,
            const std::shared_ptr<Manager::ResourceManager> &resourceManager);

        void render(const std::shared_ptr<Manager::Camera> &camera, const glm::mat4 &ortho, float dt,
            const glm::mat4 &parentTransform) const override;

        void update(float dt, uint64_t frameId) override;

        void addItem(const std::shared_ptr<MeshNode3D> &item, const glm::vec3 &color, const std::string &name);

        void hideItems() const;

        void showItems() const;

        void hideItem(const std::string &name) const;

        void showItem(const std::string &name) const;

        void clearItems();

    private:
        std::map<std::string, std::shared_ptr<RadarItem> > items;
    };
} // Model

#endif //SNAKE3_RADARMESHNODE2D_H