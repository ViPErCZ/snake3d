#ifndef SNAKE3_EATMODEL_H
#define SNAKE3_EATMODEL_H

#include "../../../Manager/ShaderManager.h"
#include "../../../ItemsDto/Eat.h"
#include "Utils/Mesh.h"

using namespace Manager;
using namespace ItemsDto;
using namespace ModelUtils;
using namespace std;

namespace Model {

    class EatModel {
    public:
        explicit EatModel(Eat *eat);
        virtual ~EatModel();
        [[nodiscard]] const map<Vao *, Mesh *> &getMeshes() const;

    protected:
        Eat* eat{};
        map<Vao*, Mesh*> meshes;
    };

} // Model

#endif //SNAKE3_EATMODEL_H
