#ifndef SNAKE3_RESOURCEMANAGER_H
#define SNAKE3_RESOURCEMANAGER_H

#include "../stdafx.h"
#include <vector>
#include <iostream>
#include <mutex>
#include <unordered_map>
#include <memory>
#include "../ItemsDto/ObjItem.h"
#include "../Resource/TextureLoader.h"
#include "TextureManager.h"
#include "ShaderManager.h"

using namespace std;
using namespace ItemsDto;
using namespace Resource;

namespace Manager {

    class ResourceManager {
    public:
        virtual ~ResourceManager();
        void addTexture(const string& name, const shared_ptr<TextureManager>& res);
        void addShader(const string& name, const shared_ptr<ShaderManager>& res);
        void addModel(const string& name, shared_ptr<ObjItem> res);
        TextureManager* getTexture(const string &name);
        ShaderManager* getShader(const string &name);
        ObjItem* getModel(const string &name);
        bool Release();
    protected:
        mutable std::mutex mutex {};
        std::unordered_map<std::string, std::shared_ptr<TextureManager>> texture;
        std::unordered_map<std::string, std::shared_ptr<ShaderManager>> shader;
        std::unordered_map<std::string, std::shared_ptr<ObjItem>> model;
    };

} // Manager

#endif //SNAKE3_RESOURCEMANAGER_H
