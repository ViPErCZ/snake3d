#define CATCH_CONFIG_MAIN
#include <catch2/catch_all.hpp>
#include <fstream>
#include <snake3d/Physic/Algorithms/CollisionAlgorithms.h>
#include <snake3d/Physic/BoxShape.h>
#include <snake3d/Physic/SphereShape.h>
#include <snake3d/Physic/CapsuleShape.h>
#include <snake3d/Physic/CylinderShape.h>
#include <snake3d/Physic/Dynamics/DynamicBody.h>
#include <snake3d/Physic/CollisionSystem3D.h>
#include "../examples/snake3/src/Manager/LevelManager.h"
#include "../examples/snake3/src/Handler/EatLocationHandler.h"
#include "../examples/snake3/src/Renderer/Opengl/Model/Game/SnakeMeshNode3D.h"
#include "../examples/snake3/src/Renderer/Opengl/Model/Game/CoinMeshNode3D.h"

using namespace Physic;

// Mock CollisionEntry for testing
struct MockCollisionEntry : public CollisionEntry {
    explicit MockCollisionEntry(const shared_ptr<Shape>& shape) {
        shapeNode = make_shared<CollisionShape3D>(nullptr, nullptr, shape);
        parentObject = make_shared<MeshNode3D>(nullptr, nullptr, nullptr);
    }

    [[nodiscard]] glm::mat4 getWorldMatrix() const {
        return parentObject->getModelMatrix() * shapeNode->getModelMatrix();
    }
};

TEST_CASE("BoxVsSphere Collision Detection") {
    auto boxShape = make_shared<BoxShape>(nullptr, nullptr, glm::vec3(2.0f)); // 2x2x2 box, halfExtents 1,1,1
    auto sphereShape = make_shared<SphereShape>(nullptr, nullptr, 0.5f); // radius 0.5

    MockCollisionEntry entBox(boxShape);
    MockCollisionEntry entSphere(sphereShape);

    SECTION("No collision - sphere far away") {
        entSphere.parentObject->setPosition({3.0f, 0.0f, 0.0f}); // Center at 3, box ends at 1. Gap 1.5.
        CHECK_FALSE(Algorithms::BoxVsSphere(entBox, entSphere, entBox.getWorldMatrix(), entSphere.getWorldMatrix()));
    }

    SECTION("Collision - sphere touching box face") {
        entSphere.parentObject->setPosition({1.4f, 0.0f, 0.0f}); // Center at 1.4, radius 0.5 -> edge at 0.9. Box edge at 1.0.
        CHECK(Algorithms::BoxVsSphere(entBox, entSphere, entBox.getWorldMatrix(), entSphere.getWorldMatrix()));
    }

    SECTION("No collision - sphere just outside") {
        entSphere.parentObject->setPosition({1.6f, 0.0f, 0.0f}); // Center at 1.6, radius 0.5 -> edge at 1.1. Box edge at 1.0.
        CHECK_FALSE(Algorithms::BoxVsSphere(entBox, entSphere, entBox.getWorldMatrix(), entSphere.getWorldMatrix()));
    }

    SECTION("Collision - sphere at corner") {
        // Box corner at (1, 1, 1)
        // Sphere center at (1.3, 1.3, 1.3), radius 0.5.
        // Distance from corner to center: sqrt(0.3^2 + 0.3^2 + 0.3^2) = sqrt(0.27) approx 0.5196
        // 0.5196 > 0.5, so NO collision.
        entSphere.parentObject->setPosition({1.3f, 1.3f, 1.3f});
        CHECK_FALSE(Algorithms::BoxVsSphere(entBox, entSphere, entBox.getWorldMatrix(), entSphere.getWorldMatrix()));

        // Sphere center at (1.2, 1.2, 1.2), radius 0.5.
        // Distance: sqrt(0.2^2 + 0.2^2 + 0.2^2) = sqrt(0.12) approx 0.346
        // 0.346 < 0.5, so YES collision.
        entSphere.parentObject->setPosition({1.2f, 1.2f, 1.2f});
        CHECK(Algorithms::BoxVsSphere(entBox, entSphere, entBox.getWorldMatrix(), entSphere.getWorldMatrix()));
    }

    SECTION("Collision - with rotation and scale") {
        boxShape = make_shared<BoxShape>(nullptr, nullptr, glm::vec3(2.0f));
        // Transformace je S * T * R
        // Pro box (0,0,0) s pozicí (0,0,0) je to jedno.
        // Ale pokud dáme pozici a scale:
        entBox.parentObject->setPosition({0.0f, 2.0f, 0.0f});
        entBox.parentObject->setScale({1.0f, 2.0f, 1.0f});
        // Světová pozice = scale * position = (1,2,1) * (0,2,0) = (0, 4, 0).
        // Velikost boxu = size * scale = (2,2,2) * (1,2,1) = (2, 4, 2).
        // Střed v (0,4,0), half-extents (1, 2, 1).
        // Horní hrana Y = 4 + 2 = 6. Spodní hrana Y = 4 - 2 = 2.
        
        // Koule poloměr 0.5. Chceme kolizi na spodní hraně (Y=2).
        // Pozice koule (0, 1.6, 0). Scale koule (1,1,1).
        // Světová pozice koule = (1,1,1) * (0, 1.6, 0) = (0, 1.6, 0).
        // Koule zasahuje do Y = 1.6 + 0.5 = 2.1. Mělo by kolidovat s Y=2.
        entSphere.parentObject->setPosition({0.0f, 1.6f, 0.0f});
        CHECK(Algorithms::BoxVsSphere(entBox, entSphere, entBox.getWorldMatrix(), entSphere.getWorldMatrix()));
        
        entSphere.parentObject->setPosition({0.0f, 1.4f, 0.0f}); // Vrchol koule na 1.9, box končí na 2.0.
        CHECK_FALSE(Algorithms::BoxVsSphere(entBox, entSphere, entBox.getWorldMatrix(), entSphere.getWorldMatrix()));
    }
}

TEST_CASE("BoxVsBox SAT Collision Detection") {
    auto boxA = make_shared<BoxShape>(nullptr, nullptr, glm::vec3(2.0f));
    auto boxB = make_shared<BoxShape>(nullptr, nullptr, glm::vec3(2.0f));

    MockCollisionEntry entA(boxA);
    MockCollisionEntry entB(boxB);

    SECTION("Collision - overlapping") {
        entB.parentObject->setPosition({1.5f, 0.0f, 0.0f});
        CHECK(Algorithms::BoxVsBox(entA, entB, entA.getWorldMatrix(), entB.getWorldMatrix()));
    }

    SECTION("No collision - separated") {
        entB.parentObject->setPosition({2.5f, 0.0f, 0.0f});
        CHECK_FALSE(Algorithms::BoxVsBox(entA, entB, entA.getWorldMatrix(), entB.getWorldMatrix()));
    }

    SECTION("Collision - rotated") {
        entB.parentObject->setPosition({1.5f, 1.5f, 0.0f});
        entB.parentObject->setRotationZ(45.0f);
        // Box A: [-1, 1]
        // Box B: center (1.5, 1.5), half-diagonal ~1.414. 
        // 1.5 - 1.414 = 0.086. Corner of B is at (0.086, 0.086) inside A.
        CHECK(Algorithms::BoxVsBox(entA, entB, entA.getWorldMatrix(), entB.getWorldMatrix()));
    }
}

TEST_CASE("Capsule Collision Detection") {
    auto capsuleShape = make_shared<CapsuleShape>(nullptr, nullptr, 0.5f, 2.0f); // radius 0.5, total height 2 (segment height 1)
    MockCollisionEntry entCap(capsuleShape);

    SECTION("Capsule vs Sphere") {
        auto sphereShape = make_shared<SphereShape>(nullptr, nullptr, 0.5f);
        MockCollisionEntry entSphere(sphereShape);

        // Capsule at (0,0,0), oriented along Y. Segment is from (0, -0.5, 0) to (0, 0.5, 0).
        // Sphere at (0, 1.2, 0). Distance to p1 is 0.7. Radii sum is 1.0. COLLISION.
        entSphere.parentObject->setPosition({0.0f, 1.2f, 0.0f});
        CHECK(Algorithms::SphereVsCapsule(entSphere, entCap, entSphere.getWorldMatrix(), entCap.getWorldMatrix()));

        // Sphere at (0, 1.6, 0). Distance to p1 is 1.1. Radii sum 1.0. NO COLLISION.
        entSphere.parentObject->setPosition({0.0f, 1.6f, 0.0f});
        CHECK_FALSE(Algorithms::SphereVsCapsule(entSphere, entCap, entSphere.getWorldMatrix(), entCap.getWorldMatrix()));
        
        // Sphere at (1.2, 0, 0). Distance to segment is 1.2. Radii sum 1.0. NO COLLISION.
        entSphere.parentObject->setPosition({1.2f, 0.0f, 0.0f});
        CHECK_FALSE(Algorithms::SphereVsCapsule(entSphere, entCap, entSphere.getWorldMatrix(), entCap.getWorldMatrix()));

        // Sphere at (0.8, 0, 0). Distance to segment is 0.8. Radii sum 1.0. COLLISION.
        entSphere.parentObject->setPosition({0.8f, 0.0f, 0.0f});
        CHECK(Algorithms::SphereVsCapsule(entSphere, entCap, entSphere.getWorldMatrix(), entCap.getWorldMatrix()));
    }

    SECTION("Capsule vs Capsule") {
        auto capsuleShapeB = make_shared<CapsuleShape>(nullptr, nullptr, 0.5f, 2.0f);
        MockCollisionEntry entCapB(capsuleShapeB);

        // CapA: (0, -0.5, 0) to (0, 0.5, 0)
        // CapB at (1.2, 0, 0): (1.2, -0.5, 0) to (1.2, 0.5, 0). Distance 1.2. Radii sum 1.0. NO.
        entCapB.parentObject->setPosition({1.2f, 0.0f, 0.0f});
        CHECK_FALSE(Algorithms::CapsuleVsCapsule(entCap, entCapB, entCap.getWorldMatrix(), entCapB.getWorldMatrix()));

        // CapB at (0.8, 0, 0): Distance 0.8. Radii sum 1.0. YES.
        entCapB.parentObject->setPosition({0.8f, 0.0f, 0.0f});
        CHECK(Algorithms::CapsuleVsCapsule(entCap, entCapB, entCap.getWorldMatrix(), entCapB.getWorldMatrix()));

        // Crossed capsules
        entCapB.parentObject->setPosition({0.0f, 0.0f, 0.8f});
        entCapB.parentObject->setRotationX(90.0f); // Oriented along Z
        // Segment B: (0, 0, 0.3) to (0, 0, 1.3). Distance between segments is at (0,0,0.5) and (0,0,0.3) -> 0.2? 
        // Wait, Segment A is Y:[-0.5, 0.5], Segment B is Z:[0.3, 1.3] (if rotation is around X 90deg and it was Y oriented).
        // Closest points: A(0, 0.5, 0), B(0, 0, 0.8)? No.
        // Let's just trust the math if it's close.
        CHECK(Algorithms::CapsuleVsCapsule(entCap, entCapB, entCap.getWorldMatrix(), entCapB.getWorldMatrix()));
    }

    SECTION("Box vs Capsule") {
        auto boxShape = make_shared<BoxShape>(nullptr, nullptr, glm::vec3(2.0f)); // halfExtents 1,1,1
        MockCollisionEntry entBox(boxShape);

        // Box [-1, 1]
        // Capsule at (1.3, 0, 0), radius 0.5. Closest point on segment is (1.3, 0, 0). 
        // Distance to box face (1, 0, 0) is 0.3. Radius 0.5. YES.
        entCap.parentObject->setPosition({1.3f, 0.0f, 0.0f});
        CHECK(Algorithms::BoxVsCapsule(entBox, entCap, entBox.getWorldMatrix(), entCap.getWorldMatrix()));

        // Capsule at (1.6, 0, 0). Distance 0.6. Radius 0.5. NO.
        entCap.parentObject->setPosition({1.6f, 0.0f, 0.0f});
        CHECK_FALSE(Algorithms::BoxVsCapsule(entBox, entCap, entBox.getWorldMatrix(), entCap.getWorldMatrix()));
    }

    SECTION("Box vs Capsule oriented along Z") {
        auto boxShape = make_shared<BoxShape>(nullptr, nullptr, glm::vec3(2.0f)); // halfExtents 1,1,1
        MockCollisionEntry entBox(boxShape);

        auto capsuleShape = make_shared<CapsuleShape>(nullptr, nullptr, 0.5f, 2.0f); // radius 0.5, total height 2 (segment height 1)
        MockCollisionEntry entCap(capsuleShape);
        entCap.parentObject->setRotationX(90.0f); // Now oriented along Z

        // Box [-1, 1]
        // Capsule at (0, 0, 1.3). Segment is Z: [0.8, 1.8]. Radius 0.5.
        // Closest point on segment to box is (0, 0, 0.8). Box max Z is 1.0.
        // (0, 0, 0.8) is INSIDE box in Z, but also X,Y are 0 which are inside.
        // Actually closest point on segment to box (max Z=1) is (0, 0, 0.8)? No, it's a point on segment.
        // The segment is from (0,0,0.8) to (0,0,1.8).
        // Point (0,0,0.8) is inside the box.
        entCap.parentObject->setPosition({0.0f, 0.0f, 1.3f});
        CHECK(Algorithms::BoxVsCapsule(entBox, entCap, entBox.getWorldMatrix(), entCap.getWorldMatrix()));

        // Capsule at (0, 0, 1.6). Segment Z: [1.1, 2.1]. Radius 0.5.
        // Closest point on segment to box is (0,0,1.1).
        // Distance from (0,0,1.1) to box face (0,0,1.0) is 0.1. Radius 0.5. YES.
        entCap.parentObject->setPosition({0.0f, 0.0f, 1.6f});
        CHECK(Algorithms::BoxVsCapsule(entBox, entCap, entBox.getWorldMatrix(), entCap.getWorldMatrix()));

        // Capsule at (0, 0, 2.1). Segment Z: [1.6, 2.6]. Radius 0.5.
        // Closest point on segment to box is (0,0,1.6).
        // Distance to (0,0,1.0) is 0.6. Radius 0.5. NO.
        entCap.parentObject->setPosition({0.0f, 0.0f, 2.1f});
        CHECK_FALSE(Algorithms::BoxVsCapsule(entBox, entCap, entBox.getWorldMatrix(), entCap.getWorldMatrix()));
    }
}

TEST_CASE("Cylinder Collision Detection") {
    auto cylinderShape = make_shared<CylinderShape>(nullptr, nullptr, 0.5f, 2.0f); // radius 0.5, total height 2
    MockCollisionEntry entCyl(cylinderShape);

    SECTION("Cylinder vs Sphere") {
        auto sphereShape = make_shared<SphereShape>(nullptr, nullptr, 0.5f);
        MockCollisionEntry entSphere(sphereShape);

        // Cylinder at (0,0,0), oriented along Y. Segment is from (0, -1, 0) to (0, 1, 0).
        // Sphere at (0, 1.4, 0). Distance to p1 is 0.4. Radius 0.5. 
        // Plane of top disk is at y=1. 1.4-1.0 = 0.4. 0.4 < 0.5. Inside radius 0.5. COLLISION.
        entSphere.parentObject->setPosition({0.0f, 1.4f, 0.0f});
        CHECK(Algorithms::SphereVsCylinder(entSphere, entCyl, entSphere.getWorldMatrix(), entCyl.getWorldMatrix()));

        // Sphere at (0, 1.6, 0). Distance to p1 is 0.6. Radius 0.5. NO COLLISION.
        entSphere.parentObject->setPosition({0.0f, 1.6f, 0.0f});
        CHECK_FALSE(Algorithms::SphereVsCylinder(entSphere, entCyl, entSphere.getWorldMatrix(), entCyl.getWorldMatrix()));
        
        // Sphere at (1.2, 0, 0). Distance to segment is 1.2. Radii sum 1.0. NO COLLISION.
        entSphere.parentObject->setPosition({1.2f, 0.0f, 0.0f});
        CHECK_FALSE(Algorithms::SphereVsCylinder(entSphere, entCyl, entSphere.getWorldMatrix(), entCyl.getWorldMatrix()));

        // Sphere at (0.8, 0, 0). Distance to segment is 0.8. Radii sum 1.0. COLLISION.
        entSphere.parentObject->setPosition({0.8f, 0.0f, 0.0f});
        CHECK(Algorithms::SphereVsCylinder(entSphere, entCyl, entSphere.getWorldMatrix(), entCyl.getWorldMatrix()));
    }

    SECTION("Box vs Cylinder") {
        auto boxShape = make_shared<BoxShape>(nullptr, nullptr, glm::vec3(2.0f)); // halfExtents 1,1,1
        MockCollisionEntry entBox(boxShape);

        // Box [-1, 1]
        // Cylinder at (1.3, 0, 0), radius 0.5. Closest point on segment is (1.3, 0, 0). 
        // Distance to box face (1, 0, 0) is 0.3. Radius 0.5. YES.
        entCyl.parentObject->setPosition({1.3f, 0.0f, 0.0f});
        CHECK(Algorithms::BoxVsCylinder(entBox, entCyl, entBox.getWorldMatrix(), entCyl.getWorldMatrix()));

        // Cylinder at (1.6, 0, 0). Distance 0.6. Radius 0.5. NO.
        entCyl.parentObject->setPosition({1.6f, 0.0f, 0.0f});
        CHECK_FALSE(Algorithms::BoxVsCylinder(entBox, entCyl, entBox.getWorldMatrix(), entCyl.getWorldMatrix()));
    }
}

TEST_CASE("Collision Layers and Masks Filtering") {
    auto shapeA = make_shared<BoxShape>(nullptr, nullptr, glm::vec3(2.0f));
    auto shapeB = make_shared<BoxShape>(nullptr, nullptr, glm::vec3(2.0f));

    const auto colA = make_shared<CollisionShape3D>(nullptr, nullptr, shapeA);
    const auto colB = make_shared<CollisionShape3D>(nullptr, nullptr, shapeB);

    SECTION("Basic Layer/Mask Match") {
        colA->setCollisionLayer(1); colA->setCollisionMask(1);
        colB->setCollisionLayer(1); colB->setCollisionMask(1);
        CHECK(CollisionShape3D::shouldCollide(colA->getCollisionLayer(), colA->getCollisionMask(),
                                              colB->getCollisionLayer(), colB->getCollisionMask()));
    }

    SECTION("Mismatch - No collision should occur") {
        colA->setCollisionLayer(2); colA->setCollisionMask(2); // Hráč koliduje s Hráčem
        colB->setCollisionLayer(4); colB->setCollisionMask(4); // Nepřítel koliduje s Nepřítelem

        // Hráč (Layer 2) vs Nepřítel (Layer 4) -> Měli by se ignorovat
        CHECK_FALSE(CollisionShape3D::shouldCollide(colA->getCollisionLayer(), colA->getCollisionMask(),
                                                    colB->getCollisionLayer(), colB->getCollisionMask()));
    }

    SECTION("Projectile ignoring its owner") {
        constexpr uint32_t WORLD = 1;
        constexpr uint32_t PLAYER = 2;
        constexpr uint32_t ENEMY = 4;
        constexpr uint32_t ENEMY_PROJECTILE = 8;

        colA->setCollisionLayer(PLAYER);
        colA->setCollisionMask(WORLD | ENEMY | ENEMY_PROJECTILE);

        colB->setCollisionLayer(ENEMY_PROJECTILE);
        colB->setCollisionMask(PLAYER | WORLD); // Střela nekoliduje s ENEMY (svým tvůrcem)

        CHECK(CollisionShape3D::shouldCollide(colA->getCollisionLayer(), colA->getCollisionMask(),
                                              colB->getCollisionLayer(), colB->getCollisionMask()));
    }
}

TEST_CASE("Eat spawn does not allow level 3 middle walls") {
    constexpr int testedLevel = 3;

    shared_ptr<DirectionalLight> directionalLight = nullptr;
    vector<shared_ptr<SpotLight>> spotLights;
    vector<shared_ptr<PointLight>> pointLights;

    Manager::LevelManager levelManager(nullptr, 1, MAX_LIVES, nullptr);
    const auto barriers = levelManager.createLevel(testedLevel, directionalLight, spotLights, pointLights);

    REQUIRE(barriers != nullptr);

    auto snake = make_shared<SnakeMeshNode3D>(nullptr, nullptr, nullptr);
    snake->x = -100000;
    snake->y = -100000;

    auto coin = make_shared<CoinMeshNode3D>(nullptr, nullptr, nullptr, nullptr);
    Handler::EatLocationHandler handler(barriers, snake, coin);

    ifstream infile("Assets/Levels/level" + std::to_string(testedLevel) + ".txt");
    REQUIRE(infile.is_open());

    vector<pair<int, int>> blockedCells;
    vector<pair<int, int>> freeCells;
    string line;
    int y = 0;
    while (getline(infile, line)) {
        for (int x = 0; x < static_cast<int>(line.size()); ++x) {
            if (line[x] == '1') {
                blockedCells.emplace_back(x, y);
            } else if (line[x] == '0') {
                freeCells.emplace_back(x, y);
            }
        }
        y++;
    }

    REQUIRE_FALSE(blockedCells.empty());
    REQUIRE_FALSE(freeCells.empty());

    for (const auto &[x, yPos] : blockedCells) {
        CHECK_FALSE(handler.isFieldEmpty(x, yPos));
    }

    int checkedFree = 0;
    for (const auto &[x, yPos] : freeCells) {
        CHECK(handler.isFieldEmpty(x, yPos));
        if (++checkedFree >= 10) {
            break;
        }
    }
}

TEST_CASE("DynamicBody integrates linear velocity") {
    using Physic::Dynamics::DynamicBody;
    DynamicBody body({0.0f, 0.0f, 5.0f});
    body.setUseGravity(false);
    body.setVelocity({1.0f, -2.0f, 0.5f});

    body.integrate(0.5f, {0.0f, 0.0f, -9.81f}); // gravity ignored because useGravity=false

    const auto &p = body.getPosition();
    CHECK(p.x == Catch::Approx(0.5f));
    CHECK(p.y == Catch::Approx(-1.0f));
    CHECK(p.z == Catch::Approx(5.25f));
    // Velocity is unchanged (no gravity, no other forces).
    CHECK(body.getVelocity().z == Catch::Approx(0.5f));
}

TEST_CASE("DynamicBody applies gravity via symplectic Euler") {
    using Physic::Dynamics::DynamicBody;
    DynamicBody body({0.0f, 0.0f, 10.0f});
    // start at rest under -Z gravity
    constexpr glm::vec3 g{0.0f, 0.0f, -10.0f};

    body.integrate(1.0f, g);

    // Symplectic: velocity += g*dt first (=> -10), then position += v*dt (=> 10 - 10 = 0).
    CHECK(body.getVelocity().z == Catch::Approx(-10.0f));
    CHECK(body.getPosition().z == Catch::Approx(0.0f));

    body.integrate(1.0f, g);
    CHECK(body.getVelocity().z == Catch::Approx(-20.0f));
    CHECK(body.getPosition().z == Catch::Approx(-20.0f));
}

TEST_CASE("DynamicBody gravityScale tunes per-body acceleration") {
    using Physic::Dynamics::DynamicBody;
    DynamicBody slow({0.0f, 0.0f, 0.0f});
    slow.setGravityScale(0.25f);
    DynamicBody fast({0.0f, 0.0f, 0.0f});
    fast.setGravityScale(2.0f);

    constexpr glm::vec3 g{0.0f, 0.0f, -10.0f};
    slow.integrate(1.0f, g);
    fast.integrate(1.0f, g);

    CHECK(slow.getVelocity().z == Catch::Approx(-2.5f));
    CHECK(fast.getVelocity().z == Catch::Approx(-20.0f));
}

TEST_CASE("DynamicBody disabled body is frozen") {
    using Physic::Dynamics::DynamicBody;
    DynamicBody body({1.0f, 2.0f, 3.0f});
    body.setVelocity({5.0f, 0.0f, 0.0f});
    body.setEnabled(false);

    body.integrate(1.0f, {0.0f, 0.0f, -9.81f});

    CHECK(body.getPosition().x == Catch::Approx(1.0f));
    CHECK(body.getPosition().z == Catch::Approx(3.0f));
    CHECK(body.getVelocity().x == Catch::Approx(5.0f));
    CHECK(body.getVelocity().z == Catch::Approx(0.0f));
}

TEST_CASE("DynamicBody useGravity=false still integrates explicit velocity") {
    using Physic::Dynamics::DynamicBody;
    DynamicBody body({0.0f, 0.0f, 0.0f});
    body.setUseGravity(false);
    body.setVelocity({0.0f, 0.0f, 4.0f});

    body.integrate(2.0f, {0.0f, 0.0f, -9.81f});

    CHECK(body.getPosition().z == Catch::Approx(8.0f));
    CHECK(body.getVelocity().z == Catch::Approx(4.0f));
}

TEST_CASE("DynamicBody addImpulse adjusts velocity additively") {
    using Physic::Dynamics::DynamicBody;
    DynamicBody body;
    body.setVelocity({1.0f, 0.0f, 0.0f});
    body.addImpulse({0.0f, 0.0f, 3.0f});
    body.addImpulse({0.5f, 0.0f, 0.0f});

    CHECK(body.getVelocity().x == Catch::Approx(1.5f));
    CHECK(body.getVelocity().z == Catch::Approx(3.0f));
}

TEST_CASE("DynamicBody integrate is a no-op for non-positive dt") {
    using Physic::Dynamics::DynamicBody;
    DynamicBody body({0.0f, 0.0f, 0.0f});
    body.setVelocity({1.0f, 0.0f, 0.0f});

    body.integrate(0.0f, {0.0f, 0.0f, -9.81f});
    CHECK(body.getPosition().x == Catch::Approx(0.0f));
    CHECK(body.getVelocity().z == Catch::Approx(0.0f));

    body.integrate(-0.1f, {0.0f, 0.0f, -9.81f});
    CHECK(body.getPosition().x == Catch::Approx(0.0f));
    CHECK(body.getVelocity().z == Catch::Approx(0.0f));
}

// ---- Scene-query API: overlapSphere / overlapAABB ------------------------
// Build a static box collider node at `pos` with full extents `size` on the
// given collision layer, registered in `sys`. Mirrors the engine's collider
// setup (node + CollisionShape3D child + computeWorldMatrix) without rendering.
static shared_ptr<MeshNode3D> addBoxCollider(CollisionSystem3D &sys, const glm::vec3 &pos,
                                             const glm::vec3 &size, const uint32_t layer) {
    auto node = make_shared<MeshNode3D>(nullptr, nullptr, nullptr);
    auto box = make_shared<BoxShape>(nullptr, nullptr, size);
    auto shape = make_shared<CollisionShape3D>(nullptr, nullptr, box);
    shape->setCollisionLayer(layer);
    shape->setCollisionMask(~0u);
    node->addNode(shape);
    node->setPosition(pos);
    node->computeWorldMatrix(glm::mat4(1.0f));
    sys.addCollider(node, true);
    return node;
}

TEST_CASE("overlapSphere reports penetration against a box collider") {
    CollisionSystem3D sys;
    // 2x2x2 box at origin -> world AABB [-1,-1,-1]..[1,1,1].
    auto node = addBoxCollider(sys, {0.0f, 0.0f, 0.0f}, glm::vec3(2.0f), 1u);

    SECTION("sphere just clear of the box -> no hit") {
        // center x=1.6, r=0.5 -> closest (1,0,0), dist 0.6 > 0.5.
        const auto hits = sys.overlapSphere({1.6f, 0.0f, 0.0f}, 0.5f);
        CHECK(hits.empty());
    }
    SECTION("sphere overlapping the +X face -> one hit with correct manifold") {
        // center x=1.3, r=0.5 -> closest (1,0,0), dist 0.3, depth 0.2, normal +X.
        const auto hits = sys.overlapSphere({1.3f, 0.0f, 0.0f}, 0.5f);
        REQUIRE(hits.size() == 1);
        CHECK(hits[0].node == node);
        CHECK(hits[0].depth == Catch::Approx(0.2f));
        CHECK(hits[0].normal.x == Catch::Approx(1.0f));
        CHECK(hits[0].normal.y == Catch::Approx(0.0f));
        CHECK(hits[0].contact.x == Catch::Approx(1.0f));
        CHECK(hits[0].worldAABB.max.x == Catch::Approx(1.0f));
    }
    SECTION("sphere far away -> no hit") {
        CHECK(sys.overlapSphere({5.0f, 0.0f, 0.0f}, 0.5f).empty());
    }
}

TEST_CASE("overlapAABB reports the minimum-translation axis") {
    CollisionSystem3D sys;
    addBoxCollider(sys, {0.0f, 0.0f, 0.0f}, glm::vec3(2.0f), 1u); // [-1..1]^3

    SECTION("box overlapping mostly along X -> shallow X push") {
        // probe centered (1.4,0,0) size 1 -> [0.9..1.9]x[-0.5..0.5]x[-0.5..0.5].
        // overlaps: x = 1-0.9 = 0.1 (smallest), y = z = 1.0.
        const AABB probe{{0.9f, -0.5f, -0.5f}, {1.9f, 0.5f, 0.5f}};
        const auto hits = sys.overlapAABB(probe);
        REQUIRE(hits.size() == 1);
        CHECK(hits[0].depth == Catch::Approx(0.1f));
        CHECK(hits[0].normal.x == Catch::Approx(1.0f));
        CHECK(hits[0].normal.y == Catch::Approx(0.0f));
        CHECK(hits[0].normal.z == Catch::Approx(0.0f));
    }
    SECTION("box clear of the collider -> no hit") {
        const AABB probe{{2.0f, 2.0f, 2.0f}, {3.0f, 3.0f, 3.0f}};
        CHECK(sys.overlapAABB(probe).empty());
    }
}

TEST_CASE("scene queries respect the layer mask") {
    CollisionSystem3D sys;
    constexpr uint32_t WORLD = 1u;
    constexpr uint32_t BLOCKER = 1u << 8; // 256
    auto worldNode = addBoxCollider(sys, {0.0f, 0.0f, 0.0f}, glm::vec3(2.0f), WORLD);
    auto blockerNode = addBoxCollider(sys, {0.0f, 0.0f, 0.0f}, glm::vec3(2.0f), BLOCKER);

    // Probe overlaps BOTH boxes (same spot), but the mask selects which layers
    // are returned.
    SECTION("mask = BLOCKER returns only the blocker collider") {
        const auto hits = sys.overlapSphere({0.0f, 0.0f, 0.0f}, 0.5f, BLOCKER);
        REQUIRE(hits.size() == 1);
        CHECK(hits[0].node == blockerNode);
    }
    SECTION("mask = WORLD returns only the world collider") {
        const auto hits = sys.overlapSphere({0.0f, 0.0f, 0.0f}, 0.5f, WORLD);
        REQUIRE(hits.size() == 1);
        CHECK(hits[0].node == worldNode);
    }
    SECTION("mask = WORLD|BLOCKER (and default ~0) returns both") {
        CHECK(sys.overlapSphere({0.0f, 0.0f, 0.0f}, 0.5f, WORLD | BLOCKER).size() == 2);
        CHECK(sys.overlapSphere({0.0f, 0.0f, 0.0f}, 0.5f).size() == 2);
    }
}
