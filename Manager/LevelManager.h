#ifndef SNAKE3_LEVELMANAGER_H
#define SNAKE3_LEVELMANAGER_H

#include "../ItemsDto/Barriers.h"
#include <memory>

using namespace ItemsDto;
using namespace std;

namespace Manager {

    class LevelManager {
    public:
        LevelManager(int level, int live, const shared_ptr<Barriers> &barriers);
        void setLevel(int level);
        void setLive(int live);
        void createLevel(int level);
        [[nodiscard]] int getLevel() const;
        [[nodiscard]] int getLive() const;
        [[nodiscard]] int getEatCounter() const;
        void setEatCounter(int eatCounter);

    protected:
        int level;
        int live;
        int eatCounter;
        shared_ptr<Barriers> barriers;
    };

} // Manager

#endif //SNAKE3_LEVELMANAGER_H
