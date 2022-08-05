#ifndef SNAKE3_SNAKE_H
#define SNAKE3_SNAKE_H

#define UNIT_MOVE 2
#define CUBE_SIZE 32

#include <functional>
#include "Cube.h"
#include "BaseContainer.h"

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
        eDIRECTION prevPauseDirection;
        float alpha;
        vector<std::function<bool(sSNAKE_TILE*)>> moveCallbacks;
        std::function<void(sSNAKE_TILE*)> lazyCallback;
    };

    class Snake: public BaseContainer<sSNAKE_TILE>{
    public:
        virtual ~Snake();
        void init() override;
        void addTile(eDIRECTION aDirection);
        void reset();
        Cube* getHeadTile();
        [[nodiscard]] const vector<sSNAKE_TILE *> &getItems() const override;
        [[nodiscard]] float getMaxX() const override;
        [[nodiscard]] float getMaxY() const override;
        [[nodiscard]] float getMinX() const override;
        [[nodiscard]] float getMinY() const override;

    protected:
        void release() override;
        vector<sSNAKE_TILE*> tiles;
    };

} // ItemsDto

#endif //SNAKE3_SNAKE_H
