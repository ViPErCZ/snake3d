#ifndef SNAKE3_IALBEDOMATERIAL_H
#define SNAKE3_IALBEDOMATERIAL_H

#include "../../../Manager/TextureManager.h"

using namespace std;
using namespace Manager;

class IAlbedoMaterial {
public:
    virtual ~IAlbedoMaterial() = default;

    [[nodiscard]] virtual shared_ptr<TextureManager> getAlbedo() const = 0;
};

#endif //SNAKE3_IALBEDOMATERIAL_H
