#ifndef SNAKE3_EATLOCATIONHANDLER_H
#define SNAKE3_EATLOCATIONHANDLER_H

#include "Handler/BaseHandler.h"
#include "../Renderer/Opengl/Model/Game/CoinMeshNode3D.h"
#include "../Renderer/Opengl/Model/Game/SnakeMeshNode3D.h"

using namespace ItemsDto;
using namespace Physic;
using namespace std;

namespace Handler {
    class EatLocationHandler final : public BaseHandler {
    public:
        ~EatLocationHandler() override;

        explicit EatLocationHandler(const shared_ptr<MeshNode3D> &barriers, const shared_ptr<SnakeMeshNode3D> &snake,
                                    const shared_ptr<CoinMeshNode3D> &eat);

        void onDefaultHandler() override;

        void onFirstPlaceHandler() const;

        void onCheckPlaceHandler() const;

        void onCleanHandler();

        void rePosition() const;

        void fixVirtualPosition(const glm::vec3 &pos) const;

        [[nodiscard]] bool isFieldEmpty(int x, int y) const;

        void clearBarriers();

        void setBarriers(const shared_ptr<MeshNode3D> &barriers) { this->barriers = barriers; }

    protected:
        shared_ptr<MeshNode3D> barriers;
        shared_ptr<SnakeMeshNode3D> snake;
        shared_ptr<CoinMeshNode3D> eat;
        int counter;

        void addTile();

        [[nodiscard]] glm::vec2 getPosition() const;
    };
} // Handler

#endif //SNAKE3_EATLOCATIONHANDLER_H
