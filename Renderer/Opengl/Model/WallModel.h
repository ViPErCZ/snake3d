#ifndef SNAKE3_WALLMODEL_H
#define SNAKE3_WALLMODEL_H

#include "../../../Manager/ShaderManager.h"
#include "../../../ItemsDto/ObjWall.h"
#include "Utils/Mesh.h"

using namespace Manager;
using namespace ItemsDto;
using namespace ModelUtils;
using namespace std;

namespace Model {

    class WallModel {
    public:
        explicit WallModel(ObjWall *wall);
        virtual ~WallModel();
        [[nodiscard]] Mesh *getMesh() const;
        [[nodiscard]] Vao *getVao() const;

    protected:
        ObjWall* wall{};
        Vao* vao;
        Mesh* mesh;
    };

} // Model

#endif //SNAKE3_WALLMODEL_H
