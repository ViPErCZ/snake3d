#ifndef SNAKE3_RADAR_H
#define SNAKE3_RADAR_H

#include "BaseItem.h"
#include "../Manager/TextureManager.h"
#include <vector>
#include <memory>

using namespace std;
using namespace Manager;

namespace ItemsDto {

    class Radar : public BaseItem {
        struct sRADAR_item {
            shared_ptr<BaseItem> item;
            shared_ptr<BaseItem> radarPresent;
            glm::vec3 color;
        };
    public:
        void updatePositions();
        void addItem(const shared_ptr<BaseItem> &item, glm::vec3 color);
        void reset();
        [[nodiscard]] const vector<sRADAR_item> &getItems() const;

    protected:
        vector<sRADAR_item> items;
    };

} // ItemsDto

#endif //SNAKE3_RADAR_H
