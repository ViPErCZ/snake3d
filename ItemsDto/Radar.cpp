#include "Radar.h"

namespace ItemsDto {
    void Radar::addItem(BaseItem *item, int presentTexture) {
        sRADAR_item radarItem{};
        radarItem.item = item;

        radarItem.radarPresent = new BaseItem();
        radarItem.radarPresent->setVisible(true);
        radarItem.radarPresent->addTexture(presentTexture);
        radarItem.radarPresent->setZoom(this->getZoom());
        radarItem.radarPresent->setWidth(3);
        radarItem.radarPresent->setHeight(3);

        items.push_back(radarItem);
    }

    const vector<Radar::sRADAR_item> &Radar::getItems() const {
        return items;
    }

    void Radar::updatePositions() {
        for(auto Iter = items.end() - 1; Iter >= items.begin(); Iter--) {
            glm::vec3 pos;
            pos.x = (((float) (this->getWidth() - SCREEN_EDGE) * 2 / 1520 /* gameField width */) *
                    (*Iter).item->getPosition().x) +
                    SCREEN_EDGE; // fix 0px init moving - no need
            pos.y = (((float) (this->getHeight() - SCREEN_EDGE) * 2 / 1520 /* gameField height */) *
                     (1520 - (*Iter).item->getPosition().y)) + 28 +
                    SCREEN_EDGE; // fix 28px init moving
            pos.z = 0;

            (*Iter).radarPresent->setPosition(pos);
        }
    }
} // ItemsDto