#ifndef SNAKE3_EATLOCATIONHANDLER_H
#define SNAKE3_EATLOCATIONHANDLER_H

#include "../ItemsDto/Snake.h"
#include "BaseHandler.h"
#include "../Physic/CollisionDetector.h"
#include "../ItemsDto/Eat.h"
#include <random>

using namespace ItemsDto;
using namespace Physic;
using namespace std;

namespace Handler {

    class EatLocationHandler : public BaseHandler {
    public:
        ~EatLocationHandler() override;
        explicit EatLocationHandler(Barriers* barriers, Snake* snake, Eat* eat);
        void onDefaultHandler() override;
        void onFirstPlaceHandler();
        void onCheckPlaceHandler();
        bool rePosition();
    protected:
        Snake* snake{};
        Barriers* barriers;
        Eat* eat{};
        bool isFieldEmpty(int x, int y);
        void addTile();
        glm::vec2 getPosition();
    };

} // Handler

#endif //SNAKE3_EATLOCATIONHANDLER_H
