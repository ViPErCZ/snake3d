#ifndef SNAKE3_GAMEFIELDMODEL_H
#define SNAKE3_GAMEFIELDMODEL_H

#include "../../../Manager/ShaderManager.h"
#include "../../../ItemsDto/GameField.h"
#include "Utils/Mesh.h"

using namespace Manager;
using namespace ItemsDto;
using namespace ModelUtils;
using namespace std;

namespace Model {

    class GameFieldModel {
    public:
        explicit GameFieldModel(BaseItem *gameField);
        virtual ~GameFieldModel();
        [[nodiscard]] Mesh *getMesh() const;
        [[nodiscard]] const Vao &getVao() const;
        [[nodiscard]] BaseItem *getGameField() const;

    protected:
        BaseItem* gameField{};
        Mesh* mesh;
        Vao vao;
    };

} // Model

#endif //SNAKE3_GAMEFIELDMODEL_H
