#define CATCH_CONFIG_MAIN
#define CATCH_CONFIG_MAIN

#include "../App.h"
#include <catch2/catch_all.hpp>
#include <Renderer/Opengl/Model/Standard/ArrayMesh.h>

using namespace Handler;
using namespace Manager;

TEST_CASE( "Checking if field is empty to place food..." ) {

    const auto contextState = make_shared<ContextState>();
    const auto coinMesh = make_shared<ArrayMesh>(ArrayMesh(nullptr));
    const auto eat = make_shared<CoinMeshNode3D>(contextState, coinMesh, nullptr);
    const auto levelManager = make_shared<LevelManager>(contextState, 1, MAX_LIVES, nullptr);
    auto light = make_shared<DirectionalLight>();
    auto spotLights = vector<shared_ptr<SpotLight> >{};
    auto pointLights = vector<shared_ptr<PointLight> >{};
    const auto levelBoxes = levelManager->createLevel(2, light, spotLights, pointLights);
    const auto snake = make_shared<SnakeMeshNode3D>(contextState, coinMesh, nullptr);
    snake->respawn();
    const auto eatLocation = new EatLocationHandler(levelBoxes, snake, eat);

    REQUIRE( levelBoxes->getChildren().size() + 1 == 40 ); // 1x parent + children
    REQUIRE( eatLocation->isFieldEmpty(4, 24) == false );
    REQUIRE( eatLocation->isFieldEmpty(5, 24) == false );
    REQUIRE( eatLocation->isFieldEmpty(4, 25) == true );
    REQUIRE( eatLocation->isFieldEmpty(5, 25) == true );
}