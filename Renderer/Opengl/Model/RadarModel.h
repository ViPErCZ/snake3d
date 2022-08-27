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
        [[nodiscard]] const map<Vao *, Mesh *> &getMeshes() const;

    protected:
        void createVertices(BaseItem* item);
        Radar* radar{};
        map<Vao*, Mesh*> meshes;
    };

} // Model

#endif //SNAKE3_RADARMODEL_H
