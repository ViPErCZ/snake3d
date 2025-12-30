#ifndef SNAKE3_TEXTUREMANAGER_H
#define SNAKE3_TEXTUREMANAGER_H

#include <vector>

using namespace std;

namespace Manager {
    class TextureManager final {
    public:
        explicit TextureManager() = default;

        explicit TextureManager(unsigned int id);
        
        ~TextureManager();

        void addTexture(unsigned int id);

        void bind() const;

        void cubeBind(int index = 0) const;

        void bind(int index, int item = 0);

        void bindArr(int index, int item);

        void unbind(int index) const;

        void unbind() const;

        [[nodiscard]] bool hasTexture() const;

    protected:
        vector<unsigned int> textures;
    };
} // Manager

#endif //SNAKE3_TEXTUREMANAGER_H
