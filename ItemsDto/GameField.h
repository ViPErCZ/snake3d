#ifndef SNAKE3_GAMEFIELD_H
#define SNAKE3_GAMEFIELD_H

#include <vector>
#include "BaseItem.h"
#include "Cube.h"

using namespace std;

namespace ItemsDto {

    class GameField : public BaseItem {
    public:
        virtual ~GameField();
        void Init();
        [[nodiscard]] const vector<Cube *> &getTiles() const;
    protected:
        void Release();
        vector<Cube*> tile;
    };

} // ItemsDto

#endif //SNAKE3_GAMEFIELD_H
