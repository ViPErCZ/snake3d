#ifndef SNAKE3_SNAKEMESHNODE3D_H
#define SNAKE3_SNAKEMESHNODE3D_H

#include "../Standard/MeshNode3D.h"

namespace Model {
    class SnakeMeshNode3D final : public MeshNode3D {
    public:
        enum eDIRECTION {
            NONE = -1,
            STOP = 0,
            LEFT = 1,
            RIGHT = 2,
            UP = 3,
            DOWN = 4,
            CRASH = 100,
            PAUSE = 200,
        };

        using MeshNode3D::MeshNode3D;

        explicit SnakeMeshNode3D(const shared_ptr<StandardMesh> &mesh,
                                 const shared_ptr<ResourceManager> &resourceManager);

        void respawn();

        void setDirectionalLight(const shared_ptr<DirectionalLight> &directional_light) override;

        void setDirection(eDIRECTION direction);

        void stop(bool stop) const;

        [[nodiscard]] eDIRECTION getDirection() const;

    private:
        shared_ptr<StandardMaterial> tileMaterial;
        eDIRECTION direction = NONE;
    };
} // Model

#endif //SNAKE3_SNAKEMESHNODE3D_H
