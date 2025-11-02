#define CATCH_CONFIG_MAIN
#define CATCH_CONFIG_MAIN

#include "../App.h"
#include <catch2/catch_all.hpp>
#include <cstdint>
#include <Renderer/Opengl/Model/Standard/ArrayMesh.h>

using namespace Handler;
using namespace Manager;

TEST_CASE( "Checking if field is empty to place food..." ) {

    // const auto geometry = make_shared<BaseItem>(BaseItem());
    // const auto coinMesh = make_shared<ArrayMesh>(ArrayMesh(geometry, nullptr));
    // auto eat = make_shared<CoinMeshNode3D>(coinMesh, nullptr);
    // auto barriers = make_shared<Barriers>();
    // auto radar = make_shared<Radar>();
    // auto snake = make_shared<SnakeMeshNode3D>(coinMesh, nullptr);
    // snake->respawn();
    // auto eatLocation = new EatLocationHandler(barriers, snake, eat, radar);
    // auto levelManager = new LevelManager(1, 3, barriers);
    // levelManager->createLevel(2);
    //
    // REQUIRE( barriers->getItems().size() == 40 );
    // REQUIRE( eatLocation->isFieldEmpty(4, 24) == false );
    // REQUIRE( eatLocation->isFieldEmpty(5, 24) == false );
    // REQUIRE( eatLocation->isFieldEmpty(4, 25) == true );
    // REQUIRE( eatLocation->isFieldEmpty(5, 25) == true );
}