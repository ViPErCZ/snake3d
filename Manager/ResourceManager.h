#ifndef SNAKE3_RESOURCEMANAGER_H
#define SNAKE3_RESOURCEMANAGER_H

#include "../stdafx.h"
#include <vector>
#include <map>
#include <iostream>

using namespace std;

namespace Manager {

    class ResourceManager {
    public:
        virtual ~ResourceManager();
        bool createTexture(const char* path, const string& name, GLint filter = GL_LINEAR_MIPMAP_LINEAR);
        GLuint getTexture(const string &name);
        bool Release();
    protected:
        map<string, GLuint> textures;
    };

} // Manager

#endif //SNAKE3_RESOURCEMANAGER_H
