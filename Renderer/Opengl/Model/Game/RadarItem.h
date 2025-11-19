#ifndef SNAKE3_RADARITEM_H
#define SNAKE3_RADARITEM_H

#include <memory>
#include <glm/vec3.hpp>
#include "../Standard/MeshNode3D.h"
#include "../Standard/2D/MeshNode2D.h"

namespace Model {
    class RadarItem {
    public:
        RadarItem(const shared_ptr<ResourceManager> &resourceManager,
            const shared_ptr<MeshNode3D> &mesh, const glm::vec3 &color, const std::string &name);
        void update();
        shared_ptr<MeshNode2D> getRadarItem();
        bool hasChangedSize() const;
        std::string getName() const;
        int getItemsCount() const;
        const vector<shared_ptr<MeshNode3D>> &getChildren() const;
        glm::vec3 getColor() const;
    private:
        shared_ptr<MeshNode3D> mesh;
        shared_ptr<MeshNode2D> radarItem;
        int itemsCount;
        bool changedSize;
        std::string name;
        glm::vec3 color;
    };
} // Model

#endif //SNAKE3_RADARITEM_H