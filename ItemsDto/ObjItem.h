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
        ~ObjItem();
        bool load(const string& filename);
        [[nodiscard]] const vector<glm::vec3> &getVertices() const;
        [[nodiscard]] const vector<glm::vec2> &getUvs() const;
        [[nodiscard]] const vector<glm::vec3> &getNormals() const;
        const vector<glm::vec3> &getIndexedVertices() const;
        const vector<glm::vec2> &getIndexedUvs() const;
        const vector<glm::vec3> &getIndexedNormals() const;
        [[nodiscard]] GLuint getVertexBuffer() const;
        [[nodiscard]] GLuint getUvBuffer() const;
        [[nodiscard]] GLuint getNormalBuffer() const;
        [[nodiscard]] GLuint getElementBuffer() const;
        [[nodiscard]] const vector<unsigned short> &getIndices() const;

    protected:
        void generateBuffer();
        GLuint vertexBuffer;
        GLuint uvBuffer;
        GLuint normalBuffer;
        GLuint elementBuffer;
        std::vector<unsigned short> indices;
        vector<glm::vec3> vertices;
        vector<glm::vec2> uvs;
        vector<glm::vec3> normals;
        std::vector<glm::vec3> indexed_vertices;
        std::vector<glm::vec2> indexed_uvs;
        std::vector<glm::vec3> indexed_normals;
    };

} // ItemsDto

#endif //SNAKE3_OBJITEM_H
