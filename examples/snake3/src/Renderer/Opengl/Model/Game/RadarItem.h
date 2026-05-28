#ifndef SNAKE3_RADARITEM_H
#define SNAKE3_RADARITEM_H

#include <memory>
#include <glm/vec3.hpp>
#include "Renderer/Opengl/Model/Standard/MeshNode3D.h"
#include "Renderer/Opengl/Model/Standard/2D/MeshNode2D.h"

namespace Model {
    class RadarItem {
    public:
        RadarItem(const std::shared_ptr<Tools::ContextState> &contextState, const std::shared_ptr<Manager::ResourceManager> &resourceManager,
            const std::shared_ptr<MeshNode3D> &mesh, const glm::vec3 &color, std::string name);
        void update();
        std::shared_ptr<MeshNode2D> getRadarItem();
        [[nodiscard]] bool hasChangedSize() const;
        [[nodiscard]] bool hasReferenceHasOnlyRadar() const;
        [[nodiscard]] std::string getName() const;
        [[nodiscard]] const std::vector<std::shared_ptr<MeshNode3D>> &getChildren() const;
        [[nodiscard]] glm::vec3 getColor() const;
    private:
        std::shared_ptr<MeshNode3D> mesh;
        std::shared_ptr<MeshNode2D> radarItem;
        bool changedSize;
        std::string name;
        glm::vec3 color;
    };
} // Model

#endif //SNAKE3_RADARITEM_H
