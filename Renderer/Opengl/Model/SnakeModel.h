#ifndef SNAKE3_SNAKEMODEL_H
#define SNAKE3_SNAKEMODEL_H

#include "../../../Manager/ShaderManager.h"
#include "../../../ItemsDto/Snake.h"
#include "Utils/Mesh.h"

using namespace Manager;
using namespace ItemsDto;
using namespace ModelUtils;
using namespace std;

namespace Model {

    class SnakeModel {
    public:
        explicit SnakeModel(Snake *snake);
        virtual ~SnakeModel();
        [[nodiscard]] const map<Vao *, Mesh *> &getMeshes() const;
        void addTiles(int tilesCount);

    protected:
        void generateMeshes(const sSNAKE_TILE *item);
        Snake* snake{};
        map<Vao*, Mesh*> meshes;
    };

} // Model

#endif //SNAKE3_SNAKEMODEL_H
