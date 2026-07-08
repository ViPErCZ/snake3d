#ifndef SNAKE3_TRANSFORM_H
#define SNAKE3_TRANSFORM_H

#include <memory>
#include <glm/fwd.hpp>
#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/ext/matrix_transform.hpp>

namespace Node3D {
    class Transform {
    public:
        Transform() = default;

        virtual ~Transform() = default;

        void setPosition(const glm::vec3 &position);

        void setPosition(const std::shared_ptr<Transform> &object);

        [[nodiscard]] const glm::vec3 &getPosition() const;

        [[nodiscard]] virtual glm::mat4 getModelMatrix() const;

        // World-space transform. Pro plain Transform (bez scene-graph parenta)
        // je world == local. MeshNode3D override vrací worldMatrixCache, kde
        // už je započítán celý parent chain. Camera::focusOn ho potřebuje, aby
        // mířila na skutečné světové umístění (např. collision shape jako
        // child barrelu se nepočítá od (0,0,0)).
        [[nodiscard]] virtual glm::mat4 getWorldMatrix() const { return getModelMatrix(); }

        [[nodiscard]] const glm::vec3 &getScale() const;

        void setScale(const glm::vec3 &scale);

        void setRotationX(float rotation_x);

        void setRotationY(float rotation_y);

        void setRotationZ(float rotation_z);

        // E1: orient by a quaternion instead of Euler X/Y/Z. Once set, getModelMatrix uses
        // the quaternion; any later setRotationX/Y/Z reverts to the Euler path. Lets callers
        // align a node to an arbitrary direction (decals/billboards/aim) without Euler math.
        void setRotationQuat(const glm::quat &q);

        // Convenience: rotate so the node's local +Y axis points along `normal` (shortest arc).
        // This is the face-normal of QuadMesh3D-style surfaces -> used to lay decals flat on
        // any surface. Falls back to identity for a zero vector.
        void setRotationToNormal(const glm::vec3 &normal);

        [[nodiscard]] float getRotationX() const;

        [[nodiscard]] float getRotationY() const;

        [[nodiscard]] float getRotationZ() const;

        void setTransform(const std::shared_ptr<Transform> &transform);

        // Dirty tag pro skip recompute v scene graph (computeWorldMatrix
        // short-circuit). Setters níže nastaví true; volající (MeshNode3D)
        // smaže po recompute. Static colliders (2300+ floor cells) ho pak
        // udrží false a celá ich subtree se přeskočí.
        [[nodiscard]] bool isTransformDirty() const { return transformDirty; }
        void clearTransformDirty() { transformDirty = false; }

    protected:
        glm::vec3 position{};
        glm::vec3 scale{1.0f, 1.0f, 1.0f};
        float rotationX = 0;
        float rotationY = 0;
        float rotationZ = 0;
        glm::quat orientation{1.0f, 0.0f, 0.0f, 0.0f}; // used only when useQuat (E1)
        bool useQuat = false;                           // false => legacy Euler path (default)
        bool transformDirty = true;
    };
} // Node3D

#endif //SNAKE3_TRANSFORM_H
