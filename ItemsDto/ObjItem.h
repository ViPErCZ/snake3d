#ifndef SNAKE3_OBJITEM_H
#define SNAKE3_OBJITEM_H
#define TINYOBJLOADER_IMPLEMENTATION // define this in only *one* .cc
//#define TINYOBJLOADER_USE_MAPBOX_EARCUT

#include "BaseItem.h"
#include "../Manager/VboIndexer.h"
#include <cstdio>

namespace ItemsDto {

    class ObjItem : public BaseItem {
    public:
        bool load(const string& filename);
        [[nodiscard]] const vector<glm::vec3> &getVertices() const;
        [[nodiscard]] const vector<glm::vec2> &getUvs() const;
        [[nodiscard]] const vector<glm::vec3> &getNormals() const;
        [[nodiscard]] const vector<glm::vec3> &getIndexedVertices() const;
        [[nodiscard]] const vector<glm::vec2> &getIndexedUvs() const;
        [[nodiscard]] const vector<glm::vec3> &getIndexedNormals() const;
        [[nodiscard]] const vector<glm::vec3> &getIndexedTangents() const;
        [[nodiscard]] const vector<glm::vec3> &getIndexedBiTangents() const;
        [[nodiscard]] const vector<unsigned int> &getIndices() const;

    protected:
        void generateBuffer();
        std::vector<unsigned int> indices;
        vector<glm::vec3> vertices;
        vector<glm::vec2> uvs;
        vector<glm::vec3> normals;
        vector<glm::vec3> tangents;
        vector<glm::vec3> biTangents;
        std::vector<glm::vec3> indexed_vertices;
        std::vector<glm::vec2> indexed_uvs;
        std::vector<glm::vec3> indexed_normals;
        std::vector<glm::vec3> indexed_tangents;
        std::vector<glm::vec3> indexed_biTangents;
    };

} // ItemsDto

#endif //SNAKE3_OBJITEM_H
