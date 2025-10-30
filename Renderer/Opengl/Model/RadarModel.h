#ifndef SNAKE3_RADARMODEL_H
#define SNAKE3_RADARMODEL_H

#include "../../../ItemsDto/Radar.h"
#include "Utils/Mesh.h"

using namespace Manager;
using namespace ItemsDto;
using namespace ModelUtils;
using namespace std;

namespace Model {

    class RadarModel final {
    public:
        explicit RadarModel(const shared_ptr<Radar> &radar);
        [[nodiscard]] shared_ptr<Mesh> getMesh() const;

    protected:
        void createVertices();
        shared_ptr<Radar> radar{};
        shared_ptr<Mesh> mesh{};
    };

} // Model

#endif //SNAKE3_RADARMODEL_H
