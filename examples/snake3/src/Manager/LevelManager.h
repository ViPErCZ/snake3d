#ifndef SNAKE3_LEVELMANAGER_H
#define SNAKE3_LEVELMANAGER_H

#define MAX_POINT 3
#define MAX_LIVES 4
#define START_LEVEL 9

#include <memory>
#include <vector>
#include <glm/glm.hpp>

#include <snake3d/Physic/CollisionSystem3D.h>
#include "Renderer/Opengl/Model/Standard/MeshNode3D.h"

using namespace Physic;
using namespace CollisionShape;
using namespace Model;
using namespace Lights;
using namespace Tools;
using namespace Manager;
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
        [[nodiscard]] const std::vector<glm::ivec2> &getHoles() const { return holes; }
        [[nodiscard]] bool isVoidAt(int virtualX, int virtualY) const;

    protected:
        void resolveBoxShape(const shared_ptr<MeshNode3D> &boxNode3D);
        void resolveFloorShape(const shared_ptr<MeshNode3D> &floorNode);
        int level;
        int live;
        int eatCounter;
        shared_ptr<ResourceManager> resourceManager;
        shared_ptr<ContextState> contextState;
        shared_ptr<CollisionSystem3D> collisionSystem;
        std::vector<glm::ivec2> holes;
    };

} // Manager

#endif //SNAKE3_LEVELMANAGER_H
