#ifndef SNAKE3_CUBE_H
#define SNAKE3_CUBE_H

#include "BaseItem.h"

namespace ItemsDto {

    class Cube : public BaseItem  {
    public:
        [[nodiscard]] float getCurrentAlpha() const;
        void setCurrentAlpha(float currentAlpha);
    protected:
        float currentAlpha = 1.0f;
    };
} // ItemsDto

#endif //SNAKE3_CUBE_H
