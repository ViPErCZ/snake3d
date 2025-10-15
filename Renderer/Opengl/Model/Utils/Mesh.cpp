#include "Mesh.h"

#include <utility>

namespace ModelUtils {
    Mesh::Mesh(const vector<Vertex> &vertices, const vector<unsigned int> &indices, bool hasBones, string name)
        : vertices(vertices), indices(indices), hasBones(hasBones), name(std::move(name)), localMin(+FLT_MAX),
          localMax(-FLT_MIN) {

        vao = nullptr;

        for (const auto &v: vertices) {
            localMin = glm::min(localMin, v.position);
            localMax = glm::max(localMax, v.position);
        }
    }

    Mesh::~Mesh() {
        delete vao;
    }

    const vector<GLuint> &Mesh::getIndices() const {
        return indices;
    }

    const vector<Vertex> &Mesh::getVertices() const {
        return vertices;
    }

    void Mesh::bind() {
        initialize();
        vao->bind();
    }

    const glm::mat4 &Mesh::getGlobalTransformation() const {
        return globalTransformation;
    }

    void Mesh::setGlobalTransformation(const glm::mat4 &globalTransformation) {
        Mesh::globalTransformation = globalTransformation;
    }

    void Mesh::initialize() {
        if (vao != nullptr) {
            return;
        }

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
        vao->linkAttribI(vbo, 6, 4, GL_INT, sizeof(Vertex), (void *) offsetof(Vertex, BoneIDs));
        vao->linkAttrib(vbo, 7, 4, GL_FLOAT, sizeof(Vertex), (void *) offsetof(Vertex, Weights));
        // Unbind all to prevent accidentally modifying them
        vao->unBind();
        vbo.unBind();
        ebo.unBind();
    }

    bool Mesh::isHasBones() const {
        return hasBones;
    }

    const string &Mesh::getName() const {
        return name;
    }

    glm::vec3 Mesh::getMin(const glm::mat4 &modelMatrix) const {
        const glm::vec3 corners[8] = {
            {localMin.x, localMin.y, localMin.z},
            {localMin.x, localMin.y, localMax.z},
            {localMin.x, localMax.y, localMin.z},
            {localMin.x, localMax.y, localMax.z},
            {localMax.x, localMin.y, localMin.z},
            {localMax.x, localMin.y, localMax.z},
            {localMax.x, localMax.y, localMin.z},
            {localMax.x, localMax.y, localMax.z}
        };

        glm::vec3 worldMin(+FLT_MAX);
        for (auto corner: corners) {
            glm::vec4 worldPos = modelMatrix * glm::vec4(corner, 1.0f);
            worldMin = glm::min(worldMin, glm::vec3(worldPos));
        }
        return worldMin;
    }

    glm::vec3 Mesh::getMax(const glm::mat4 &modelMatrix) const {
        glm::vec3 corners[8] = {
            {localMin.x, localMin.y, localMin.z},
            {localMin.x, localMin.y, localMax.z},
            {localMin.x, localMax.y, localMin.z},
            {localMin.x, localMax.y, localMax.z},
            {localMax.x, localMin.y, localMin.z},
            {localMax.x, localMin.y, localMax.z},
            {localMax.x, localMax.y, localMin.z},
            {localMax.x, localMax.y, localMax.z}
        };

        glm::vec3 worldMax(-FLT_MAX);
        for (auto corner: corners) {
            glm::vec4 worldPos = modelMatrix * glm::vec4(corner, 1.0f);
            worldMax = glm::max(worldMax, glm::vec3(worldPos));
        }
        return worldMax;
    }
} // ModelUtils
