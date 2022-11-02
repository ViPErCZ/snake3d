#ifndef SNAKE3_RESOURCEMANAGER_H
#define SNAKE3_RESOURCEMANAGER_H

#include "../stdafx.h"
#include <vector>
#include <map>
#include <iostream>
#include <mutex>
#include <unordered_map>
#include <memory>
#include "../ItemsDto/ObjItem.h"

using namespace std;
using namespace ItemsDto;

namespace Manager {

    class ResourceManager {
    public:
        virtual ~ResourceManager();
        bool createTexture(const char* path, const string& name, GLint filter = GL_NEAREST_MIPMAP_LINEAR);
        GLuint getTexture(const string &name);
        bool Release();
        void addModel(const string& name, shared_ptr<ObjItem> res);
        ObjItem* getModel(const string &name);
    protected:
        map<string, GLuint> textures;
        mutable std::mutex mutex {};
        std::unordered_map<std::string, std::shared_ptr<GLuint>> resource;
        std::unordered_map<std::string, std::shared_ptr<ObjItem>> model;
    };

} // Manager

#endif //SNAKE3_RESOURCEMANAGER_H
