#ifndef SNAKE3_TEXTUREMANAGER_H
#define SNAKE3_TEXTUREMANAGER_H

#include <vector>
#include <assimp/texture.h>

using namespace std;

namespace Manager {
    class TextureManager final {
    public:
        explicit TextureManager() = default;

        explicit TextureManager(unsigned int id);

        explicit TextureManager(const aiTexel *buffer, unsigned int size);

        explicit TextureManager(const unsigned char *buffer, unsigned int size, unsigned int width, unsigned int height, unsigned int numColCh);

        ~TextureManager();

        void addTexture(unsigned int id);
        void replaceTexture(unsigned int id, int item = 0);

        void bind() const;

        void cubeBind(int index = 0) const;

        void bind(int index, int item = 0);

        void bindArr(int index, int item);

        void unbind(int index) const;

        void unbind() const;

        void lazyLoad(bool isAlbedo);

        [[nodiscard]] bool hasTexture() const;

    protected:
        vector<unsigned int> textures;
        vector<unsigned char> buffer;
        int widthImg, heightImg, numColCh;
    };
} // Manager

#endif //SNAKE3_TEXTUREMANAGER_H
