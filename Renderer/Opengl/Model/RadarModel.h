#ifndef SNAKE3_RADARMODEL_H
#define SNAKE3_RADARMODEL_H

#include "../../../ItemsDto/Radar.h"
#include "Utils/Mesh.h"

using namespace Manager;
using namespace ItemsDto;
using namespace ModelUtils;
using namespace std;

namespace Model {

    class RadarModel {
    public:
        explicit RadarModel(Radar *radar);
        virtual ~RadarModel();

        Mesh *getMesh() const;

    protected:
        void createVertices();
        Radar* radar{};
        Mesh* mesh{};
    };

} // Model

#endif //SNAKE3_RADARMODEL_H
