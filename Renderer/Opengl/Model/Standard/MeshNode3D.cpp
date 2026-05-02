#include "MeshNode3D.h"

#include "../Collision/CollisionShape3D.h"

namespace Model {
    MeshNode3D::MeshNode3D(const shared_ptr<ContextState> &contextState, const shared_ptr<StandardMesh> &mesh,
                           const shared_ptr<ResourceManager> &resourceManager)
        : contextState(contextState), mesh(mesh), resourceManager(resourceManager), transformDetached(false),
          childrenChangedSignal(false), lastUpdatedFrame(0), worldMatrixCache(1.0f) {
    }

    MeshNode3D::~MeshNode3D() {
        children.clear();
        collisionShapes.clear();
        spotLights.clear();
        pointLights.clear();
    }

    shared_ptr<StandardMesh> MeshNode3D::getMesh() const {
        return mesh;
    }

    void MeshNode3D::addNode(const std::shared_ptr<MeshNode3D> &node) {
        node->parent = shared_from_this();
        node->depth = this->depth + 1;
        if (this->depth > 20) {
            throw std::runtime_error("Depth limit reached. Maximum nesting scene nodes is 20");
        }
        children.push_back(node);

        if (const auto collisionShape = std::dynamic_pointer_cast<CollisionShape::CollisionShape3D>(node)) {
            collisionShapes.push_back(collisionShape);
        } else {
            childrenChangedSignal = true;
        }
    }

    void MeshNode3D::render(const shared_ptr<Camera> &camera, const glm::mat4 &projection, const float dt,
                            const glm::mat4 &parentTransform, const bool shadows) {
        if (visible) {
            glm::mat4 finalTransform = worldMatrixCache;
            if constexpr (isDebug) {
                finalTransform = parentTransform * this->getModelMatrix();
            }
            contextState->setBlendingMode(mesh->getBlending());
            contextState->setDepthTest(mesh->getDepthTest());
            contextState->setDepthWrite(mesh->getDepthWrite());
            if (mesh != nullptr) {
                mesh->render(camera, projection, 1, finalTransform, shadows);
            }

            for (auto &node: children) {
                const bool isCollisionShapeNode = node->isCollisionShapeNode();
                const glm::mat4 childParentTransform = (transformDetached && !isCollisionShapeNode)
                                                           ? glm::mat4(1.0f)
                                                           : finalTransform;
                node->render(camera, projection, dt, childParentTransform, shadows);
            }
        } else if (transformDetached) {
            for (auto &node: children) {
                node->render(camera, projection, dt, glm::mat4(1.0f), shadows);
            }
        }
    }

    void MeshNode3D::update(const float dt, const uint64_t frameId) {
        if (lastUpdatedFrame == frameId) {
            return;
        }

        lastUpdatedFrame = frameId;

        if (mesh != nullptr) {
            mesh->update(dt);
        }
        for (const auto &node: children) {
            node->update(dt, frameId);
        }
        if (childrenChangedSignalCycles > 1) {
            childrenChangedSignal = false;
            childrenChangedSignalCycles = 0;
        } else if (childrenChangedSignal) {
            childrenChangedSignalCycles++;
        }
    }

    void MeshNode3D::renderShadows(const shared_ptr<Camera> &camera, const glm::mat4 &projection, const float dt,
                                   const glm::mat4 &parentTransform) const {
        if (visible) {
            const glm::mat4 finalTransform = worldMatrixCache;
            if (mesh != nullptr) {
                mesh->renderShadowMap(camera, projection, dt, finalTransform);
            }
            for (const auto &node: children) {
                const bool isCollisionShapeNode = node->isCollisionShapeNode();
                const glm::mat4 childParentTransform = (transformDetached && !isCollisionShapeNode)
                                                           ? glm::mat4(1.0f)
                                                           : finalTransform;
                node->renderShadows(camera, projection, dt, childParentTransform);
            }
        } else if (transformDetached) {
            for (auto &node: children) {
                node->renderShadows(camera, projection, dt, glm::mat4(1.0f));
            }
        }
    }

    const vector<shared_ptr<MeshNode3D>> &MeshNode3D::getChildren() const {
        return children;
    }

    const vector<shared_ptr<CollisionShape::CollisionShape3D>> & MeshNode3D::getCollisionShapes() const {
        return collisionShapes;
    }

    void MeshNode3D::setDirectionalLight(const shared_ptr<DirectionalLight> &directional_light) {
        directionalLight = directional_light;
    }

    void MeshNode3D::setSpotLights(const vector<shared_ptr<SpotLight>> &spot_light) {
        spotLights = spot_light;
    }

    void MeshNode3D::setPointLights(const vector<shared_ptr<PointLight>> &point_light) {
        pointLights = point_light;
    }

    void MeshNode3D::setTransformDetached(const bool transform_detached, const bool recursive) {
        transformDetached = transform_detached;

        if (recursive) {
            for (const auto &child: children) {
                child->setTransformDetached(transform_detached, recursive);
            }
        }
    }

    void MeshNode3D::animationStart(const string &name, const bool loop) {
        try {
            animation = name;
            mesh->animationPlay(name, loop);
        } catch (exception &e) {
            cout << "Mesh is not AnimationArrayMesh instance." << endl;
        }
    }

    void MeshNode3D::animationStop(const string &name) const {
        try {
            mesh->animationStop(name);
        } catch (exception &e) {
            cout << "Mesh is not AnimationArrayMesh instance." << endl;
        }
    }

    void MeshNode3D::animationPause(const string &name) const {
        try {
            mesh->animationPause(name);
        } catch (exception &e) {
            cout << "Mesh is not AnimationArrayMesh instance." << endl;
        }
    }

    void MeshNode3D::animationResume(const string &name) const {
        try {
            mesh->animationResume(name);
        } catch (exception &e) {
            cout << "Mesh is not AnimationArrayMesh instance." << endl;
        }
    }

    void MeshNode3D::disablePlanarReflection() {
        includePlanarReflection = false;
    }

    bool MeshNode3D::isIncludeInPlanarReflection() const {
        return includePlanarReflection;
    }

    void MeshNode3D::computeWorldMatrix(const glm::mat4 &parentTransform) {
        worldMatrixCache = parentTransform * this->getModelMatrix();

        for (auto &node: children) {
            const bool isCollisionShapeNode = node->isCollisionShapeNode();
            const glm::mat4 childParentTransform = (transformDetached && !isCollisionShapeNode)
                                                       ? glm::mat4(1.0f)
                                                       : worldMatrixCache;
            node->computeWorldMatrix(childParentTransform);
        }
    }

    std::shared_ptr<MeshNode3D> MeshNode3D::deepCopy() const {
        auto copyMesh = std::make_shared<StandardMesh>(*mesh);
        auto copyNode = std::make_shared<MeshNode3D>(contextState, copyMesh, resourceManager);

        copyNode->setPosition(this->getPosition());
        copyNode->setScale(this->getScale());
        copyNode->setRotationX(this->rotationX);
        copyNode->setRotationY(this->rotationY);
        copyNode->setRotationZ(this->rotationZ);

        for (auto &child: this->children) {
            auto childCopy = child->deepCopy();
            copyNode->addNode(childCopy);
        }

        return copyNode;
    }

    void MeshNode3D::make_unique() {
        const auto copy = this->deepCopy();
        *this = *copy;
    }

    bool MeshNode3D::hasChildrenChangedSignal() const {
        return childrenChangedSignal;
    }

} // Model
