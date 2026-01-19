#include <iostream>
#include <vector>

// Assimp includes
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "ObjModelLoader.h"

#include "TextureLoader.h"

namespace Resource {
    std::vector<TextureInfo> loadMaterialTextures(const aiMaterial *mat, const aiTextureType type,
                                                  const TextureType typeName, const aiScene *scene) {
        std::vector<TextureInfo> textures;

        // Projdeme všechny textury daného typu v materiálu
        for (unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
            aiString str;
            mat->GetTexture(type, i, &str);

            TextureInfo texture;
            texture.type = typeName;
            texture.path = str.C_Str();

            // Pokud cesta začíná hvězdičkou (např. "*0", "*1"), znamená to,
            // že textura je EMBEDDED (zabalená) přímo v binárním souboru.
            const aiTexture *embeddedTexture = scene->GetEmbeddedTexture(str.C_Str());
            if (embeddedTexture) {
                // Zde musíš texturu načíst z paměti (viz vysvětlení pod kódem)
                texture.texture = std::make_shared<TextureManager>(
                    TextureLoader::bindFromBuffer(embeddedTexture->pcData, embeddedTexture->mWidth));
                std::cout << "Nalezen embedded texture: " << str.C_Str() << std::endl;
            } else {
                // Textura je externí soubor na disku
                texture.texture = std::make_shared<TextureManager>(TextureLoader::loadTexture(str.C_Str()));
                std::cout << "Nalezen file texture: " << str.C_Str() << std::endl;
            }

            textures.push_back(texture);
        }
        return textures;
    }

    std::vector<std::shared_ptr<Mesh>> processAssimpScene(const aiScene* scene) {

        std::vector<std::shared_ptr<Mesh>> meshes;

        for (unsigned int m = 0; m < scene->mNumMeshes; m++) {
            aiMesh* mesh = scene->mMeshes[m];

            std::vector<Vertex> vertices;
            std::vector<unsigned int> indices;
            std::vector<TextureInfo> textures;

            // 1. VRCHOLY (Zůstává stejné jako tvoje, jen bez offsetu, protože každý mesh je teď zvlášť)
            for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
                Vertex vertex{};
                vertex.position = { mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z };

                if (mesh->HasNormals()) {
                    vertex.normal = { mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z };
                }

                if (mesh->mTextureCoords[0]) {
                    vertex.texUV = { mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y };
                } else {
                    vertex.texUV = {0.0f, 0.0f};
                }

                if (mesh->HasTangentsAndBitangents()) {
                    vertex.tangents = { mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z };
                }

                vertices.push_back(vertex);
            }

            // 2. INDEXY
            for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
                aiFace face = mesh->mFaces[i];
                for (unsigned int j = 0; j < face.mNumIndices; j++) {
                    indices.push_back(face.mIndices[j]);
                }
            }

            // 3. MATERIÁLY A TEXTURY (To hlavní!)
            if (mesh->mMaterialIndex >= 0) {
                aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

                // a) Diffuse mapy (Base Color)
                // V novějším Assimp a GLTF se Base Color mapuje často jako aiTextureType_BASE_COLOR nebo DIFFUSE
                std::vector<TextureInfo> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, TextureType::Diffuse, scene);
                textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

                // b) Normal mapy
                std::vector<TextureInfo> normalMaps = loadMaterialTextures(material, aiTextureType_NORMALS, TextureType::Normal, scene);
                textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());

                // c) EMISSIVE (Tvoje žárovka!)
                std::vector<TextureInfo> emissiveMaps = loadMaterialTextures(material, aiTextureType_EMISSIVE, TextureType::Emissive, scene);
                textures.insert(textures.end(), emissiveMaps.begin(), emissiveMaps.end());

                // aiColor3D color(0.f, 0.f, 0.f);
                // if (material->Get(AI_MATKEY_COLOR_EMISSIVE, color) == AI_SUCCESS) {
                //     const auto meshEmissiveColor = glm::vec3(color.r, color.g, color.b);
                //
                //     if (meshEmissiveColor.r > 0 || meshEmissiveColor.g > 0 || meshEmissiveColor.b > 0) {
                //         std::cout << "Mesh [" << m << "] sviti barvou: "
                //                   << meshEmissiveColor.r << ", "
                //                   << meshEmissiveColor.g << ", "
                //                   << meshEmissiveColor.b << std::endl;
                //     }
                // }
                // tady si emissive color ulozit a pracovat s nim dal

                // Poznámka: PBR Metallic/Roughness je složitější, Assimp to často dává do aiTextureType_UNKNOWN
                // nebo specifických PBR flagů, ale pro začátek stačí tyto tři.
            }

            meshes.push_back(std::make_shared<Mesh>(vertices, indices, textures));
        }

        return meshes;
    }

    // Definice flagů pro Assimp:
    // aiProcess_Triangulate: Převede polygony na trojúhelníky
    // aiProcess_GenSmoothNormals: Vypočítá normály, pokud chybí
    // aiProcess_FlipUVs: Otočí Y souřadnici textury (záleží na vašem enginu, v OpenGL často potřeba)
    // aiProcess_CalcTangentSpace: Vypočítá Tangenty/Bitangenty (nahrazuje VboIndexer)
    // aiProcess_JoinIdenticalVertices: Optimalizuje mesh a vytváří indexy (nahrazuje VboIndexer)
    static constexpr unsigned int ASSIMP_FLAGS =
        aiProcess_Triangulate |
        aiProcess_GenSmoothNormals |
        aiProcess_FlipUVs |
        aiProcess_CalcTangentSpace |
        aiProcess_JoinIdenticalVertices;

    std::vector<std::shared_ptr<Mesh>> ObjModelLoader::loadObj(const fs::path &path) {
        Assimp::Importer importer;

        // Načtení souboru
        const aiScene* scene = importer.ReadFile(path.string(), ASSIMP_FLAGS);

        // Kontrola chyb
        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
            std::cerr << "Assimp Error: " << importer.GetErrorString() << std::endl;
            exit(1);
        }

        return processAssimpScene(scene);
    }

    std::vector<std::shared_ptr<Mesh>> ObjModelLoader::loadObjFromStr(const fs::path &path, const string &str) {
        Assimp::Importer importer;

        std::string extension = path.extension().string();
        if(extension.empty()) extension = ".obj";

        const aiScene* scene = importer.ReadFileFromMemory(
            str.c_str(),
            str.length(),
            ASSIMP_FLAGS,
            extension.c_str()
        );

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
            std::cerr << "Assimp Error (from string): " << importer.GetErrorString() << std::endl;
            exit(1);
        }

        return processAssimpScene(scene);
    }

} // Resource