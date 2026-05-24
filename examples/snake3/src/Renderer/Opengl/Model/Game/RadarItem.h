#ifndef SNAKE3_RADARITEM_H
#define SNAKE3_RADARITEM_H

#include <memory>
#include <glm/vec3.hpp>
#include "Renderer/Opengl/Model/Standard/MeshNode3D.h"
#include "Renderer/Opengl/Model/Standard/2D/MeshNode2D.h"

namespace Model {
    class RadarItem {
    public:
        RadarItem(const shared_ptr<ContextState> &contextState, const shared_ptr<ResourceManager> &resourceManager,
            const shared_ptr<MeshNode3D> &mesh, const glm::vec3 &color, std::string name);
        void update();
        shared_ptr<MeshNode2D> getRadarItem();
        [[nodiscard]] bool hasChangedSize() const;
        [[nodiscard]] bool hasReferenceHasOnlyRadar() const;
        [[nodiscard]] std::string getName() const;
        [[nodiscard]] const vector<shared_ptr<MeshNode3D>> &getChildren() const;
        [[nodiscard]] glm::vec3 getColor() const;
    private:
        shared_ptr<MeshNode3D> mesh;
        shared_ptr<MeshNode2D> radarItem;
        bool changedSize;
        std::string name;
        glm::vec3 color;
    };
} // Model

#endif //SNAKE3_RADARITEM_H
