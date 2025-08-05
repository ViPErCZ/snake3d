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
            void onEventHandler(unsigned key, int scancode, const int action, int mods) override;
            void addItem(BaseItem* item);
            BaseItem * findNextItem();
        protected:
            Camera* camera = nullptr;
            vector<BaseItem*> items;
            BaseItem* activeItem = nullptr;
            BaseItem* cameraOriginalStickyPoint = nullptr;
            bool enabled;
    };

} // Handler

#endif //POSITIONHANDLER_H
