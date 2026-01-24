#ifndef SNAKE3_STREETLAMPNODE3D_H
#define SNAKE3_STREETLAMPNODE3D_H

#include "../Standard/MeshNode3D.h"

namespace Model {
    class StreetLampNode3D final : public MeshNode3D {
        public:
        explicit StreetLampNode3D(const shared_ptr<ContextState> &contextState, const shared_ptr<ResourceManager> &resourceManager);
        void init();
    };
} // Model

#endif //SNAKE3_STREETLAMPNODE3D_H