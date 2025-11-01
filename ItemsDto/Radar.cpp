#include "Radar.h"

namespace ItemsDto {

    const vector<Radar::sRADAR_item> &Radar::getItems() const {
        return items;
    }

    void Radar::updatePositions() {
        for(auto Iter = items.end() - 1; Iter >= items.begin(); --Iter) {
            glm::vec3 pos;
            pos.x = 18 + this->getWidth() / 3020 * static_cast<float>(Iter->item->x);
            pos.y = -61 + 176 - this->getHeight() / 3020 * static_cast<float>(Iter->item->y);
            pos.z = 0;

            Iter->radarPresent->setPosition(pos);
        }
    }

    void Radar::addItem(const shared_ptr<BaseItem> &item, const glm::vec3 color) {
        sRADAR_item radarItem{};
        radarItem.item = item;
        radarItem.radarPresent = make_shared<BaseItem>();
        radarItem.radarPresent->setVisible(true);
        radarItem.radarPresent->setZoom({2,2,1});
        radarItem.color = color;

        items.push_back(radarItem);
    }

    void Radar::reset() {
        items.clear();
    }

} // ItemsDto