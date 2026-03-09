#ifndef SNAKE3_LEVELMANAGER_H
#define SNAKE3_LEVELMANAGER_H

#define MAX_POINT 6
#define MAX_LIVES 4
#define START_LEVEL 3

#include <memory>

#include "../Physic/CollisionSystem3D.h"
#include "../Renderer/Opengl/Model/Standard/MeshNode3D.h"

using namespace Physic;
using namespace CollisionShape;
using namespace Model;
using namespace std;

namespace Manager {

    class LevelManager {
    public:
        LevelManager(const shared_ptr<ContextState> &contextState, int level, int live, const shared_ptr<ResourceManager> &resourceManager);
        void setLevel(int level);
        void setLive(int live);
        shared_ptr<MeshNode3D> createLevel(int level, shared_ptr<DirectionalLight> &directionalLight, const vector<shared_ptr<SpotLight> > &spotLights,
            const vector<shared_ptr<PointLight> > &pointLights);
        [[nodiscard]] int getLevel() const;
        [[nodiscard]] int getLive() const;
        [[nodiscard]] int getEatCounter() const;
        void setEatCounter(int eatCounter);
        void setCollisionSystem(const shared_ptr<CollisionSystem3D> &collisionSystem);

    protected:
        void resolveBoxShape(const shared_ptr<MeshNode3D> &boxNode3D);
        int level;
        int live;
        int eatCounter;
        shared_ptr<ResourceManager> resourceManager;
        shared_ptr<ContextState> contextState;
        shared_ptr<CollisionSystem3D> collisionSystem;
    };

} // Manager

#endif //SNAKE3_LEVELMANAGER_H
