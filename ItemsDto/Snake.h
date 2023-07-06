#ifndef SNAKE3_SNAKE_H
#define SNAKE3_SNAKE_H

#define UNIT_MOVE 0.125
#define VIRTUAL_MOVE 2 // kvuli nepresnosti float cislum pocitame virtualne v integer formatu
#define CUBE_SIZE 32

#include <functional>
#include "BaseContainer.h"
#include "ObjItem.h"
#include "Cube.h"

namespace ItemsDto {

    enum eDIRECTION {
        NONE = -1,
        STOP = 0,
        LEFT = 1,
        RIGHT = 2,
        UP = 3,
        DOWN = 4,
        CRASH = 100,
        PAUSE = 200,
    };

    struct sSNAKE_TILE {
        Cube* tile;
        eDIRECTION direction = NONE;
        eDIRECTION prevPauseDirection; // smer pred pauzou (je treba si zapamatovat smer nez doslo ke stisku pause)
        float alpha;
        vector<std::function<bool(sSNAKE_TILE*)>> moveCallbacks;
    };

    class Snake: public BaseContainer<sSNAKE_TILE>{
    public:
        virtual ~Snake();
        void init() override;
        void addTile(eDIRECTION aDirection);
        void reset();
        Cube* getHeadTile();
        [[nodiscard]] const vector<sSNAKE_TILE *> &getItems() const override;
        [[nodiscard]] int getMaxX() const override;
        [[nodiscard]] int getMaxY() const override;
        [[nodiscard]] int getMinX() const override;
        [[nodiscard]] int getMinY() const override;

    protected:
        void release() override;
        vector<sSNAKE_TILE*> tiles;
    };

} // ItemsDto

#endif //SNAKE3_SNAKE_H
