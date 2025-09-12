#ifndef SNAKE3_IALBEDOMATERIAL_H
#define SNAKE3_IALBEDOMATERIAL_H

#include "../../../Manager/TextureManager.h"

class IAlbedoMaterial {
public:
    virtual ~IAlbedoMaterial() = default;

    [[nodiscard]] virtual std::shared_ptr<Manager::TextureManager> getAlbedo() const = 0;
};

#endif //SNAKE3_IALBEDOMATERIAL_H
