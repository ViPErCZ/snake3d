#ifndef SNAKE3_OBJMODELLOADER_H
#define SNAKE3_OBJMODELLOADER_H
#define TINYOBJLOADER_IMPLEMENTATION // define this in only *one* .cc

#include <filesystem>
#include <memory>

#include "../Renderer/Opengl/Model/Utils/Mesh.h"

using namespace std;
using namespace ModelUtils;

namespace fs = std::filesystem;

namespace Resource {

    class ObjModelLoader {
    public:
        static vector<shared_ptr<Mesh>> loadObj(const fs::path& path);
        static vector<shared_ptr<Mesh>> loadObjFromStr(const fs::path& path, const string& str);
    };

} // Resource

#endif //SNAKE3_OBJMODELLOADER_H
