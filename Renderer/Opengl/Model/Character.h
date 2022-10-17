#ifndef SNAKE3_CHARACTER_H
#define SNAKE3_CHARACTER_H
#include <glm/glm.hpp>

namespace Model {

    class Character {
    public:
        [[nodiscard]] unsigned int getTextureId() const;
        void setTextureId(unsigned int textureId);
        [[nodiscard]] const glm::ivec2 &getSize() const;
        void setSize(const glm::ivec2 &size);
        [[nodiscard]] const glm::ivec2 &getBearing() const;
        void setBearing(const glm::ivec2 &bearing);
        [[nodiscard]] unsigned int getAdvance() const;
        void setAdvance(unsigned int advance);
    protected:
        unsigned int textureId;
        glm::ivec2   size;
        glm::ivec2   bearing;
        unsigned int advance;
    };

} // Model

#endif //SNAKE3_CHARACTER_H
