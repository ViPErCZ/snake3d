#ifndef SNAKE3_BARRELNODE3D_H
#define SNAKE3_BARRELNODE3D_H
#include "../Standard/MeshNode3D.h"

namespace Model {
    class BarrelNode3D final : public MeshNode3D {
        public:
        explicit BarrelNode3D(const shared_ptr<ContextState> &contextState, const shared_ptr<ResourceManager> &resourceManager);
        void init();
    };
} // Model

#endif //SNAKE3_BARRELNODE3D_H