#ifndef POSITIONHANDLER_H
#define POSITIONHANDLER_H

#include <vector>

#include "../../Manager/Camera.h"
#include "../BaseKeydownHandle.h"
#include "../../ItemsDto/BaseItem.h"

using namespace ItemsDto;
using namespace Manager;

namespace Handler {

    class PositionHandler final : public BaseKeydownHandle {
        public:
            explicit PositionHandler(Camera* camera);
            void onDefaultHandler() override;
            void onEventHandler(unsigned key, int scancode, int action, int mods) override;
            void addItem(Transform* item);
            Transform * findNextItem();
        protected:
            Camera* camera = nullptr;
            vector<Transform*> items;
            Transform* activeItem = nullptr;
            Transform* cameraOriginalStickyPoint = nullptr;
            bool enabled;
    };

} // Handler

#endif //POSITIONHANDLER_H
