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
//            (((float) (this->getWidth() - SCREEN_EDGE) * 2 / 1520 /* gameField width */) *
//                    (*Iter).item->getVirtualX()) +
//                    SCREEN_EDGE; // fix 0px init moving - no need
            pos.y = 160-88 + 176 - ((this->getHeight() / 1520) * (float)(*Iter).item->getVirtualY());
//            (((float) (this->getHeight() - SCREEN_EDGE) * 2 / 1520 /* gameField height */) *
//                     (1520 - (*Iter).item->getVirtualY())) + 28 +
//                    SCREEN_EDGE; // fix 28px init moving
            pos.z = 0;

            (*Iter).radarPresent->setPosition(pos);
        }
    }
} // ItemsDto