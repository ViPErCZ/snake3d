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

        const map<Vao *, Mesh *> &getRadarMesh() const;

        const map<Vao *, Mesh *> &getItemsMeshes() const;

    protected:
        void createVertices(BaseItem* item, map<Vao*, Mesh*>& storage);
        Radar* radar{};
        map<Vao*, Mesh*> radarMesh;
        map<Vao*, Mesh*> itemsMeshes;
    };

} // Model

#endif //SNAKE3_RADARMODEL_H
