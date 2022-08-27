#ifndef SNAKE3_RADAR_H
#define SNAKE3_RADAR_H

#include "BaseItem.h"

namespace ItemsDto {

    class Radar : public BaseItem {
        const float SCREEN_EDGE = 18.0;
        struct sRADAR_item {
            BaseItem* item;
            BaseItem* radarPresent;
        };
    public:
        void updatePositions();
        void addItem(BaseItem* item, GLuint presentTexture);
        [[nodiscard]] const vector<Radar::sRADAR_item> &getItems() const;

    protected:
        vector<sRADAR_item> items;
    };

} // ItemsDto

#endif //SNAKE3_RADAR_H
