#ifndef SNAKE3_ANIMLOADER_H
#define SNAKE3_ANIMLOADER_H

#include <snake3d/Tools/AnimItem.h>
#include <filesystem>
#include <map>
#include <memory>
#include <unordered_map>
#include <vector>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>

#include <snake3d/Renderer/Opengl/Model/Standard/Animation/AnimationPlayer.h>

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
            static std::shared_ptr<Animation::AnimationPlayer> loadObj(const fs::path &path);
        protected:
            static void processNode(const aiNode *node, const aiScene *scene, std::vector<std::shared_ptr<ModelUtils::Mesh>> &meshes, const glm::mat4 &parentTransformation,
                std::unordered_map<std::string, uint32_t>& bone_map, std::vector<std::shared_ptr<Animation::Bone> >& bones);
            static std::shared_ptr<ModelUtils::Mesh> processMesh(aiMesh *mesh, const aiScene *scene, std::unordered_map<std::string, uint32_t>& bone_map, std::vector<std::shared_ptr<Animation::Bone> >& bones);
            static glm::mat4 AiMatrix4x4ToGlm(const aiMatrix4x4 *from);
            static std::map<std::string, std::shared_ptr<Animation::AnimationClip> > loadAnimations(const aiScene* scene, std::vector<std::shared_ptr<Animation::Bone>>& bones,
                const std::unordered_map<std::string, uint32_t>& bone_map);
            static ModelUtils::Tree<uint32_t> loadAnimationTree(const aiScene* scene, std::vector<std::shared_ptr<Animation::Bone>> &bones, std::unordered_map<std::string, uint32_t> &bone_map,
                std::map<std::string, std::shared_ptr<Animation::AnimationClip> > &anim);
    };

} // Resource

#endif //SNAKE3_ANIMLOADER_H
