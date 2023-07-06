#include "RainModel.h"
#include "../../../Manager/VboIndexer.h"

Model::RainModel::RainModel(BaseItem *baseItem, unsigned int amount) : item(baseItem), amount(amount),
                                                                       lastUsedParticle(0) {

    vector<glm::vec3> vbo_vertices;
    vector<glm::vec3> vbo_normals;
    vector<glm::vec2> vbo_uvs;
    vector<glm::vec3> tangents;
    vector<glm::vec3> biTangents;

    vector<Vertex> vertices;
    vector<GLuint> indices;

    Vertex vertex{};
    vertex.position = {-1.0f, -1.0f, -1.0f};
    vertex.normal = {0.0f, 1.0f, 0.0f};
    vertex.color = {0.0f, 0.0f, 0.0f};
    vertex.texUV = {0.0f, 0.0f};

    vbo_vertices.push_back(vertex.position);
    vbo_uvs.push_back(vertex.texUV);
    vbo_normals.push_back(vertex.normal);

    vertices.push_back(vertex);
    indices.push_back(0);

    Vertex vertex2{};
    vertex2.position = {1.0f, -1.0f, -1.0f};
    vertex2.normal = {0.0f, 1.0f, 0.0f};
    vertex2.color = {0.0f, 0.0f, 0.0f};
    vertex2.texUV = {1.0f, 0.0f};

    vbo_vertices.push_back(vertex2.position);
    vbo_uvs.push_back(vertex2.texUV);
    vbo_normals.push_back(vertex2.normal);

    vertices.push_back(vertex2);
    indices.push_back(1);

    Vertex vertex3{};
    vertex3.position = {1.0f, 1.0f, -1.0f};
    vertex3.normal = {0.0f, 1.0f, 0.0f};
    vertex3.color = {0.0f, 0.0f, 0.0f};
    vertex3.texUV = {1.0f, 1.0f};

    vbo_vertices.push_back(vertex3.position);
    vbo_uvs.push_back(vertex3.texUV);
    vbo_normals.push_back(vertex3.normal);

    vertices.push_back(vertex3);
    indices.push_back(2);

    Vertex vertex4{};
    vertex4.position = {1.0f, 1.0f, -1.0f};
    vertex4.normal = {0.0f, 1.0f, 0.0f};
    vertex4.color = {0.0f, 0.0f, 0.0f};
    vertex4.texUV = {1.0f, 1.0f};

    vbo_vertices.push_back(vertex4.position);
    vbo_uvs.push_back(vertex4.texUV);
    vbo_normals.push_back(vertex4.normal);

    vertices.push_back(vertex4);
    indices.push_back(3);

    Vertex vertex5{};
    vertex5.position = {-1.0f, 1.0f, -1.0f};
    vertex5.normal = {0.0f, 1.0f, 0.0f};
    vertex5.color = {0.0f, 0.0f, 0.0f};
    vertex5.texUV = {0.0f, 1.0f};

    vbo_vertices.push_back(vertex5.position);
    vbo_uvs.push_back(vertex5.texUV);
    vbo_normals.push_back(vertex5.normal);

    vertices.push_back(vertex5);
    indices.push_back(4);

    Vertex vertex6{};
    vertex6.position = {-1.0f, -1.0f, -1.0f};
    vertex6.normal = {0.0f, 1.0f, 0.0f};
    vertex6.color = {0.0f, 0.0f, 0.0f};
    vertex6.texUV = {0.0f, 0.0f};

    vbo_vertices.push_back(vertex6.position);
    vbo_uvs.push_back(vertex6.texUV);
    vbo_normals.push_back(vertex6.normal);

    vertices.push_back(vertex6);
    indices.push_back(5);

    VboIndexer::computeTangentBasis(vbo_vertices, vbo_uvs, vbo_normals, tangents, biTangents);

    int index = 0;
    for (auto iter: tangents) {
        auto vertIter = vertices.begin() + index;
        (*vertIter).tangents = iter;
        index++;
    }

    mesh = new Mesh(vertices, indices);

    for (unsigned int i = 0; i < this->amount; ++i) {
        particles.emplace_back();
    }
}

Model::RainModel::~RainModel() {
    delete mesh;
}

void Model::RainModel::respawnParticle(Model::Particle &particle) {
    random_device rd; // obtain a random number from hardware
    mt19937 gen(rd()); // seed the generator
    uniform_int_distribution<> fields(0);
    uniform_int_distribution<> posX(1,20);
    uniform_int_distribution<> posY(0,35);
    uniform_int_distribution<> velocity(10, 100);

    float rColor = 0.5f + ((float) (fields(gen) % 100) / 100.0f);
    particle.Position.x = -0.5f + ((float)posX(gen) / 10);
    particle.Position.y = -1 + ((float)posY(gen) / 10);
    particle.Position.z = 2.0;
    particle.Color = glm::vec4(rColor);
    particle.Color.a = 0.3;
    particle.Velocity.x = (float) 1 / 10.0f;
    particle.Velocity.y = (float) 1 / 10.0f;
    particle.Velocity.z = (float) velocity(gen) * 1.4f / 10;
}

Mesh *Model::RainModel::getMesh() const {
    return mesh;
}

const vector<Model::Particle> &Model::RainModel::getParticles() const {
    return particles;
}

unsigned int Model::RainModel::firstUnusedParticle() {
    for (unsigned int i = lastUsedParticle; i < amount; ++i) {
        if (particles[i].Position.z <= -2.0f) {
            lastUsedParticle = i;

            return i;
        }
    }
    for (unsigned int i = 0; i < lastUsedParticle; ++i) {
        if (particles[i].Position.z <= -2.0f) {
            lastUsedParticle = i;

            return i;
        }
    }
    lastUsedParticle = 0;

    return -1;
}

void Model::RainModel::update(float dt, unsigned int newParticles) {
    for (unsigned int i = 0; i < newParticles; ++i) {
        unsigned int unusedParticle = firstUnusedParticle();
        if (unusedParticle == -1) {
            continue;
        }
        respawnParticle(particles[unusedParticle]);
    }
    // update all particles
    for (unsigned int i = 0; i < amount; ++i) {
        Particle &p = particles[i];
        if (p.Position.z >= -2.0f) {
            p.Position.z -= p.Velocity.z * dt;
            p.Color.a -= dt * 0.1f;
        }
    }
}
