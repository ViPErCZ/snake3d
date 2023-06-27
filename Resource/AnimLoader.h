#ifndef SNAKE3_ANIMLOADER_H
#define SNAKE3_ANIMLOADER_H

#include "../Renderer/Opengl/Model/AnimationModel.h"
#include "../ItemsDto/ObjItem.h"
#include "../ItemsDto/AnimItem.h"
#include <filesystem>
#include <memory>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

using namespace std;
using namespace Model;

namespace fs = std::filesystem;

namespace Resource {

    inline glm::mat4 convert(const aiMatrix4x4& aimat) noexcept {
        return glm::mat4{aimat.a1, aimat.b1, aimat.c1, aimat.d1,
                         aimat.a2, aimat.b2, aimat.c2, aimat.d2,
                         aimat.a3, aimat.b3, aimat.c3, aimat.d3,
                         aimat.a4, aimat.b4, aimat.c4, aimat.d4};
    }

    inline glm::vec3 convert3f(const aiVector3D& aivec) noexcept {
        return glm::vec3{aivec.x, aivec.y, aivec.z};
    }

    inline glm::vec2 convert2f(const aiVector3D& aivec) noexcept {
        return glm::vec2{aivec.x, aivec.y};
    }

    inline glm::fquat convertQuat(const aiQuaternion& aiquat) noexcept {
        return { aiquat.w, aiquat.x, aiquat.y, aiquat.z };
    }

    class AnimLoader {
        public:
            static AnimationModel* loadObj(const fs::path &path);
        protected:
            static void processNode(aiNode *node, const aiScene *scene, vector<Mesh*>* meshes, glm::mat4 parentTransformation, unordered_map<std::string, uint32_t>& bone_map, vector<Bone>& bones);
            static Mesh* processMesh(aiMesh *mesh, const aiScene *scene, unordered_map<std::string, uint32_t>& bone_map, vector<Bone>& bones);
            static glm::mat4 AiMatrix4x4ToGlm(const aiMatrix4x4 *from);
            static std::vector<Animation> loadAnimations(const aiScene* scene, std::vector<Bone>& bones, std::unordered_map<std::string, uint32_t>& bone_map);
            static Tree<uint32_t> loadAnimationTree(const aiScene* scene, std::vector<Bone>& bones, std::unordered_map<std::string, uint32_t>& bone_map, std::vector<Animation>& anim);
            static std::vector<Animation> animations2;
    };

} // Resource

#endif //SNAKE3_ANIMLOADER_H
