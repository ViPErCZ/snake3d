#ifndef SNAKE3_OBJMODELLOADER_H
#define SNAKE3_OBJMODELLOADER_H
#define TINYOBJLOADER_IMPLEMENTATION // define this in only *one* .cc

#include <filesystem>
#include <memory>

#include <snake3d/Renderer/Opengl/Model/Utils/Mesh.h>

namespace fs = std::filesystem;

namespace Resource {

    class ObjModelLoader {
    public:
        static std::vector<std::shared_ptr<ModelUtils::Mesh>> loadObj(const fs::path& path);
        static std::vector<std::shared_ptr<ModelUtils::Mesh>> loadObjFromStr(const fs::path& path, const std::string& str);
    };

} // Resource

#endif //SNAKE3_OBJMODELLOADER_H
