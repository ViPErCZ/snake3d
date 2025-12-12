#ifndef SNAKE3_LEVELMANAGER_H
#define SNAKE3_LEVELMANAGER_H

#define MAX_POINT 6
#define MAX_LIVES 4
#define START_LEVEL 2

#include <memory>

#include "../Renderer/Opengl/Model/Standard/MeshNode3D.h"

using namespace ItemsDto;
using namespace Model;
using namespace std;

namespace Manager {

    class LevelManager {
    public:
        LevelManager(int level, int live, const shared_ptr<ResourceManager> &resourceManager);
        void setLevel(int level);
        void setLive(int live);
        shared_ptr<MeshNode3D> createLevel(int level);
        [[nodiscard]] int getLevel() const;
        [[nodiscard]] int getLive() const;
        [[nodiscard]] int getEatCounter() const;
        void setEatCounter(int eatCounter);

    protected:
        int level;
        int live;
        int eatCounter;
        shared_ptr<ResourceManager> resourceManager;
    };

} // Manager

#endif //SNAKE3_LEVELMANAGER_H
