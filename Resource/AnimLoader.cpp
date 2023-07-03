#include "AnimLoader.h"

namespace Resource {
    shared_ptr<AnimationModel> AnimLoader::loadObj(const fs::path &path) {
        Assimp::Importer importer;
        const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals |
                                                       aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
        // check for errors
        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
        {
            cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << endl;
            exit(1);
        }

        vector<Mesh*> meshes;
        vector<Bone> bones;
        unordered_map<std::string, uint32_t> bone_map;

        processNode(scene->mRootNode, scene, &meshes, glm::mat4(1.0f), bone_map, bones);
        bones.reserve(bones.size() + 100);

        auto animations = loadAnimations(scene, bones, bone_map);
        auto animation_tree = loadAnimationTree(scene, bones, bone_map, animations);
        auto global_matrix = convert(scene->mRootNode->mTransformation);

        cout << "Loading OBJ file " << path << " success" << endl;
        importer.FreeScene();

        return std::make_shared<AnimationModel>(meshes, std::move(animations), std::move(bones),
                                                std::move(animation_tree), std::move(bone_map),
                                                glm::inverse(global_matrix));
    }

    std::vector<Animation> AnimLoader::loadAnimations(const aiScene* scene, std::vector<Bone>& bones, std::unordered_map<std::string, uint32_t>& bone_map) {
        std::vector<Animation> animations;

        for (uint32_t i = 0; i < scene->mNumAnimations; ++i) {
            const auto* anim = scene->mAnimations[i];

            std::string anim_name(anim->mName.C_Str());
            std::vector<AnimationNode> anim_nodes;

            for (uint32_t j = 0; j < anim->mNumChannels; ++j) {
                const auto* channel = anim->mChannels[j];

                auto bi = bone_map.find(channel->mNodeName.C_Str());
//                if (bi == bone_map.end()) {
//                    bones.emplace_back(channel->mNodeName.C_Str(), glm::mat4(1.f));
//                    bone_map.emplace(channel->mNodeName.C_Str(), bones.size() - 1);
//                }

                auto& bone = bones.at(bone_map.at(channel->mNodeName.C_Str()));
                std::vector<KeyFrame<glm::vec3>> pos_frames;
                std::vector<KeyFrame<glm::fquat>> rot_frames;
                std::vector<KeyFrame<glm::vec3>> scale_frames;

                for (uint32_t k = 0; k < channel->mNumPositionKeys; ++k) {
                    auto vec = convert3f(channel->mPositionKeys[k].mValue);
                    pos_frames.emplace_back(vec, channel->mPositionKeys[k].mTime);
                }

                for (uint32_t k = 0; k < channel->mNumRotationKeys; ++k) {
                    auto quat = convertQuat(channel->mRotationKeys[k].mValue);
                    rot_frames.emplace_back(quat, channel->mRotationKeys[k].mTime);
                }

                for (uint32_t k = 0; k < channel->mNumScalingKeys; ++k) {
                    auto vec = convert3f(channel->mScalingKeys[k].mValue);
                    scale_frames.emplace_back(vec, channel->mScalingKeys[k].mTime);
                }

                anim_nodes.emplace_back(pos_frames, rot_frames, scale_frames, bone);
            }

            animations.emplace_back(anim_name, anim->mDuration, anim->mTicksPerSecond > 0 ? anim->mTicksPerSecond : 25, anim_nodes);
        }

        return animations;
    }

    Tree<uint32_t> AnimLoader::loadAnimationTree(const aiScene* scene, std::vector<Bone>& bones, std::unordered_map<std::string, uint32_t>& bone_map, std::vector<Animation>& anim) {
        auto bone_finder = [&] (const std::string& str, std::vector<Animation>& anim) {
            if (auto bi = bone_map.find(str); bi != bone_map.end()) {
                return bi->second;
            } else {
                bones.emplace_back(str, "", glm::mat4(1.f));
                bone_map.emplace(str, bones.size() - 1);
                return static_cast<uint32_t>(bones.size() - 1);
            }
        };

        Tree<uint32_t> tree(bone_finder(scene->mRootNode->mName.C_Str(), anim));

        std::function<void(Tree<uint32_t>& tree, const aiNode*, int)> dfs;
        dfs = [&] (Tree<uint32_t>& tree, const aiNode* node, int depth) {
            bones[*tree].node_transform = convert(node->mTransformation);

            for (uint32_t i = 0; i < node->mNumChildren; ++i) {
                auto& child = tree.add(bone_finder(node->mChildren[i]->mName.C_Str(), anim));
                dfs(child, node->mChildren[i], depth + 1);
            }
        };

        dfs(tree, scene->mRootNode, 0);

        return tree;
    }

    void AnimLoader::processNode(aiNode *node, const aiScene *scene, vector<Mesh*>* meshes, glm::mat4 parentTransformation, unordered_map<std::string, uint32_t>& bone_map, vector<Bone>& bones) {
        glm::mat4 transformation = AiMatrix4x4ToGlm(&node->mTransformation);
        glm::mat4 globalTransformation = parentTransformation * transformation;

        for (unsigned int i = 0; i < node->mNumMeshes; i++) {
            aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
            Mesh* myMesh = processMesh(mesh, scene, bone_map, bones);
            myMesh->setGlobalTransformation(globalTransformation);
            meshes->push_back(myMesh);
        }
        for (unsigned int i = 0; i < node->mNumChildren; i++) {
            processNode(node->mChildren[i], scene, meshes, globalTransformation, bone_map, bones);
        }
    }

    glm::mat4 AnimLoader::AiMatrix4x4ToGlm(const aiMatrix4x4* from)
    {
        glm::mat4 to;

        to[0][0] = (GLfloat)from->a1; to[0][1] = (GLfloat)from->b1;  to[0][2] = (GLfloat)from->c1; to[0][3] = (GLfloat)from->d1;
        to[1][0] = (GLfloat)from->a2; to[1][1] = (GLfloat)from->b2;  to[1][2] = (GLfloat)from->c2; to[1][3] = (GLfloat)from->d2;
        to[2][0] = (GLfloat)from->a3; to[2][1] = (GLfloat)from->b3;  to[2][2] = (GLfloat)from->c3; to[2][3] = (GLfloat)from->d3;
        to[3][0] = (GLfloat)from->a4; to[3][1] = (GLfloat)from->b4;  to[3][2] = (GLfloat)from->c4; to[3][3] = (GLfloat)from->d4;

        return to;
    }

    Mesh* AnimLoader::processMesh(aiMesh *mesh, const aiScene *scene, unordered_map<std::string, uint32_t>& bone_map, vector<Bone>& bones) {

        std::vector<VertexBoneWeight> bone_weights;

        if (mesh->HasBones()) {
            bone_weights.resize(mesh->mNumVertices);

            for (uint32_t j = 0; j < mesh->mNumBones; ++j) {
                std::string bone_name = mesh->mBones[j]->mName.C_Str();
                std::string mesh_name = mesh->mName.C_Str();
                auto bi = bone_map.find(bone_name);
                auto offset_mat = convert(mesh->mBones[j]->mOffsetMatrix);
                uint32_t bone_index;
                if (bi == bone_map.end()) {
//                    bones.emplace_back(bone_name, offset_mat);
                    bones.emplace_back(bone_name, mesh_name, offset_mat);
                    bone_index = bones.size() - 1;
                    bone_map.insert({bone_name, bone_index});
                } else {
                    bone_index = bi->second;
                }

                for (uint32_t k = 0; k < mesh->mBones[j]->mNumWeights; ++k) {
                    auto w = mesh->mBones[j]->mWeights[k];
                    bone_weights.at(mesh->mBones[j]->mWeights[k].mVertexId).addBoneWeight(bone_index, w.mWeight);
                }
            }
        }

        if (!bone_map.empty() && (mesh->mNumVertices != bone_weights.size())) {
            //throw model_loader_error("Bone weights for some vertices does not exist, model is corrupted.");
            throw invalid_argument("Bone weights for some vertices does not exist, model is corrupted.");
        }

        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
        aiColor3D color{1.0f};
        material->Get(AI_MATKEY_COLOR_DIFFUSE, color);
        vector<Vertex> vertices;
        vector<unsigned int> indices;
        //vector<Texture> textures;

        // walk through each of the mesh's vertices
        for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
            Vertex vertex{};
            glm::vec3 vector; // we declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
            // positions
            vector.x = mesh->mVertices[i].x;
            vector.y = mesh->mVertices[i].y;
            vector.z = mesh->mVertices[i].z;
            vertex.position = vector;
            // normals
            if (mesh->HasNormals()) {
                vector.x = mesh->mNormals[i].x;
                vector.y = mesh->mNormals[i].y;
                vector.z = mesh->mNormals[i].z;
                vertex.normal = vector;
            }
            // texture coordinates
            if (mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
            {
                glm::vec2 vec;
                // a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't
                // use models where a vertex can have multiple texture coordinates so we always take the first set (0).
                vec.x = mesh->mTextureCoords[0][i].x;
                vec.y = mesh->mTextureCoords[0][i].y;
                vertex.texUV = vec;
                // tangent
                vector.x = mesh->mTangents[i].x;
                vector.y = mesh->mTangents[i].y;
                vector.z = mesh->mTangents[i].z;
                vertex.tangents = vector;
                // bitangent
                vector.x = mesh->mBitangents[i].x;
                vector.y = mesh->mBitangents[i].y;
                vector.z = mesh->mBitangents[i].z;
                vertex.biTangents = vector;
            } else {
                vertex.texUV = glm::vec2(0.0f, 0.0f);
            }

            vertex.color = glm::vec4{color.r, color.g, color.b, 0.0};
            if (!bone_weights.empty()) {
                vertex.Weights[0] = (bone_weights.begin() + i)->weight[0];
                vertex.Weights[1] = (bone_weights.begin() + i)->weight[1];
                vertex.Weights[2] = (bone_weights.begin() + i)->weight[2];
                vertex.Weights[3] = (bone_weights.begin() + i)->weight[3];
                vertex.BoneIDs[0] = (int)(bone_weights.begin() + i)->bone_index[0];
                vertex.BoneIDs[1] = (int)(bone_weights.begin() + i)->bone_index[1];
                vertex.BoneIDs[2] = (int)(bone_weights.begin() + i)->bone_index[2];
                vertex.BoneIDs[3] = (int)(bone_weights.begin() + i)->bone_index[3];
            }
            vertices.push_back(vertex);
        }
        // now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
        for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
            aiFace face = mesh->mFaces[i];
            // retrieve all indices of the face and store them in the indices vector
            for (unsigned int j = 0; j < face.mNumIndices; j++)
                indices.push_back(face.mIndices[j]);
        }

        // TODO: implementovat ???
//        for (int i = 0; i < material->GetTextureCount(aiTextureType_DIFFUSE); i++)
//        {
//            aiString str;
//            material->GetTexture(aiTextureType_DIFFUSE, i, &str);
//        }

        // 1. diffuse maps
//        vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
//        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
//        // 2. specular maps
//        vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
//        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
//        // 3. normal maps
//        std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
//        textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
//        // 4. height maps
//        std::vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
//        textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

        return new Mesh(vertices, indices, mesh->HasBones(), mesh->mName.C_Str());
    }
} // Resource