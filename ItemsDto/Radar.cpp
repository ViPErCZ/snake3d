#include "Radar.h"

namespace ItemsDto {
    void Radar::addItem(BaseItem *item, GLuint presentTexture) {
        sRADAR_item radarItem{};
        radarItem.item = item;

        radarItem.radarPresent = new BaseItem();
        radarItem.radarPresent->setVisible(true);
        radarItem.radarPresent->addTexture(presentTexture);
        radarItem.radarPresent->setZoom({2,2,1});

        items.push_back(radarItem);
    }

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
} // ItemsDto