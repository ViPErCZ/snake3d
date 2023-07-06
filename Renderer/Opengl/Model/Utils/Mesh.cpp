#include "Mesh.h"

#include <utility>

namespace ModelUtils {
    Mesh::Mesh(const vector<Vertex> &vertices, const vector<unsigned int> &indices, bool hasBones, string name)
            : vertices(vertices), indices(indices), hasBones(hasBones), name(std::move(name)) {
        vao = new Vao();
        vao->bind();
        // Generates Vertex Buffer Object and links it to vertices
        Vbo vbo(this->vertices);
        // Generates Element Buffer Object and links it to indices
        Ebo ebo(this->indices);
        // Links VBO attributes such as coordinates and colors to VAO
        vao->linkAttrib(vbo, 0, 3, GL_FLOAT, sizeof(Vertex), (void *) nullptr);
        vao->linkAttrib(vbo, 1, 3, GL_FLOAT, sizeof(Vertex), (void *) (3 * sizeof(float)));
        vao->linkAttrib(vbo, 2, 3, GL_FLOAT, sizeof(Vertex), (void *) (6 * sizeof(float)));
        vao->linkAttrib(vbo, 3, 2, GL_FLOAT, sizeof(Vertex), (void *) (9 * sizeof(float)));
        vao->linkAttrib(vbo, 4, 3, GL_FLOAT, sizeof(Vertex), (void *) (11 * sizeof(float)));
        vao->linkAttrib(vbo, 5, 3, GL_FLOAT, sizeof(Vertex), (void *) (14 * sizeof(float)));
        vao->linkAttribI(vbo, 6, 4, GL_INT, sizeof(Vertex), (void*)offsetof(Vertex, BoneIDs));
        vao->linkAttrib(vbo, 7, 4, GL_FLOAT, sizeof(Vertex), (void*)offsetof(Vertex, Weights));
        // Unbind all to prevent accidentally modifying them
        vao->unBind();
        vbo.unBind();
        ebo.unBind();
    }

    Mesh::~Mesh() {
        delete vao;
    }

    const vector<GLuint> &Mesh::getIndices() const {
        return indices;
    }

    void Mesh::bind() {
        vao->bind();
    }

    const glm::mat4 &Mesh::getGlobalTransformation() const {
        return globalTransformation;
    }

    void Mesh::setGlobalTransformation(const glm::mat4 &globalTransformation) {
        Mesh::globalTransformation = globalTransformation;
    }

    bool Mesh::isHasBones() const {
        return hasBones;
    }

    const string &Mesh::getName() const {
        return name;
    }

} // ModelUtils