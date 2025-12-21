#ifndef SNAKE3_COINMESHNODE3D_H
#define SNAKE3_COINMESHNODE3D_H

#include "../Standard/MeshNode3D.h"

namespace Model {
    class CoinMeshNode3D final : public MeshNode3D {
    public:
        using MeshNode3D::MeshNode3D;
    protected:
        double lastTime{};
    };
} // Model

#endif //SNAKE3_COINMESHNODE3D_H