#ifndef SNAKE3_OBJMODELLOADER_H
#define SNAKE3_OBJMODELLOADER_H
#define TINYOBJLOADER_IMPLEMENTATION // define this in only *one* .cc

#include "../ItemsDto/ObjItem.h"
#include <filesystem>
#include <memory>

using namespace std;
using namespace ItemsDto;

namespace fs = std::filesystem;

namespace Resource {

    class ObjModelLoader {
    public:
        static shared_ptr<ObjItem> loadObj(const fs::path& path);
    };

} // Resource

#endif //SNAKE3_OBJMODELLOADER_H
