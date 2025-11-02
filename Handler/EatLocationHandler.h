#ifndef SNAKE3_EATLOCATIONHANDLER_H
#define SNAKE3_EATLOCATIONHANDLER_H

#include "../ItemsDto/Snake.h"
#include "BaseHandler.h"
#include "../Physic/CollisionDetector.h"
#include "../ItemsDto/Radar.h"
#include "../Renderer/Opengl/Model/Game/CoinMeshNode3D.h"
#include "../Renderer/Opengl/Model/Game/SnakeMeshNode3D.h"

using namespace ItemsDto;
using namespace Physic;
using namespace std;

namespace Handler {
    class EatLocationHandler final : public BaseHandler {
    public:
        ~EatLocationHandler() override;

        explicit EatLocationHandler(const shared_ptr<Barriers> &barriers, const shared_ptr<SnakeMeshNode3D> &snake,
                                    const shared_ptr<CoinMeshNode3D> &eat, const shared_ptr<Radar> &radar);

        void onDefaultHandler() override;

        void onFirstPlaceHandler() const;

        void onCheckPlaceHandler() const;

        void onCleanHandler();

        void rePosition() const;

        [[nodiscard]] bool isFieldEmpty(int x, int y) const;

    protected:
        shared_ptr<Radar> radar;
        shared_ptr<Barriers> barriers;
        shared_ptr<SnakeMeshNode3D> snake;
        shared_ptr<CoinMeshNode3D> eat;
        int counter;

        void addTile();

        [[nodiscard]] glm::vec2 getPosition() const;
    };
} // Handler

#endif //SNAKE3_EATLOCATIONHANDLER_H
