#ifndef SNAKE3_RADAR_H
#define SNAKE3_RADAR_H

#include "BaseItem.h"
#include "../Manager/TextureManager.h"
#include <vector>

using namespace std;
using namespace Manager;

namespace ItemsDto {

    class Radar : public BaseItem {
        struct sRADAR_item {
            const BaseItem* item;
            BaseItem* radarPresent;
            const TextureManager* texture;
            glm::vec3 color;
        };
    public:
        void updatePositions();
        void addItem(const BaseItem* item, const TextureManager* texture);
        void addItem(const BaseItem* item, glm::vec3 color);
        void reset();
        [[nodiscard]] const vector<Radar::sRADAR_item> &getItems() const;

    protected:
        vector<sRADAR_item> items;
    };

} // ItemsDto

#endif //SNAKE3_RADAR_H
