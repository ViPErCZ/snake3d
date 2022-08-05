#ifndef SNAKE3_RESOURCEMANAGER_H
#define SNAKE3_RESOURCEMANAGER_H

#include "../stdafx.h"
#include <vector>
#include <iostream>

using namespace std;

namespace Manager {

    class ResourceManager {
    public:
        bool createTexture(char *aPath, GLint filter = GL_LINEAR_MIPMAP_LINEAR);
        GLuint getTexture(int texture);
        bool Release();
    protected:
    public:
        virtual ~ResourceManager();

    protected:
        vector<GLuint> textures;
    };

} // Manager

#endif //SNAKE3_RESOURCEMANAGER_H
