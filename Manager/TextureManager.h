#ifndef SNAKE3_TEXTUREMANAGER_H
#define SNAKE3_TEXTUREMANAGER_H

#include <vector>

using namespace std;

namespace Manager {

    class TextureManager {
    public:
        virtual ~TextureManager();

        void addTexture(unsigned int id);
        void bind() const;
        void cubeBind() const;
        void bind(int index, int item = 0);
        void unbind() const;
    protected:
        vector<unsigned int> textures;
    };

} // Manager

#endif //SNAKE3_TEXTUREMANAGER_H
