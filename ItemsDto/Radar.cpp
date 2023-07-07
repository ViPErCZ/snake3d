#include "Radar.h"

namespace ItemsDto {

    const vector<Radar::sRADAR_item> &Radar::getItems() const {
        return items;
    }

    void Radar::updatePositions() {
        for(auto Iter = items.end() - 1; Iter >= items.begin(); Iter--) {
            glm::vec3 pos;
            pos.x = 125-88 + 176;
            pos.x = 125-88 + ((this->getWidth() / 1520) * (float)(*Iter).item->getVirtualX());
            pos.y = 160-88 + 176 - ((this->getHeight() / 1520) * (float)(*Iter).item->getVirtualY());
            pos.z = 0;

            (*Iter).radarPresent->setPosition(pos);
        }
    }

    void Radar::addItem(const BaseItem *item, glm::vec3 color) {
        sRADAR_item radarItem{};
        radarItem.item = item;
        radarItem.radarPresent = new BaseItem();
        radarItem.radarPresent->setVisible(true);
        radarItem.radarPresent->setZoom({2,2,1});
        radarItem.color = color;

        items.push_back(radarItem);
    }

    void Radar::reset() {
        items.clear();
    }

} // ItemsDto