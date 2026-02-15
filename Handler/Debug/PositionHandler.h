#ifndef POSITIONHANDLER_H
#define POSITIONHANDLER_H

#include <memory>
#include <vector>

#include "../../Manager/Camera.h"
#include "../BaseKeydownHandle.h"
#include "../../Lights/DirectionalLight.h"

using namespace Manager;
using namespace Lights;

namespace Handler {
    class PositionHandler final : public BaseKeydownHandle {
        public:
            explicit PositionHandler(const shared_ptr<Camera> &camera);
            void onDefaultHandler() override;
            void onEventHandler(unsigned key, int scancode, int action, int mods) override;
            void addItem(const shared_ptr<Transform> &item);
            shared_ptr<Transform>  findNextItem();
        protected:
            shared_ptr<Camera> camera = nullptr;
            vector<shared_ptr<Transform> > items;
            shared_ptr<Transform> activeItem = nullptr;
            shared_ptr<Transform> cameraOriginalStickyPoint = nullptr;
            bool enabled;
    };
} // Handler

#endif //POSITIONHANDLER_H
