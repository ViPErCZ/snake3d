#include "../App.h"
#include <iostream>
#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include <cstdint>

using namespace Handler;
using namespace Manager;

TEST_CASE( "Checking if field is empty to place food..." ) {

    auto snake = new Snake;
    snake->init();
    snake->reset();
    auto eat = new Eat;
    auto barriers = new Barriers;
    auto radar = new Radar;
    auto eatLocation = new EatLocationHandler(barriers, snake, eat, radar);
    auto levelManager = new LevelManager(1, 3, barriers);
    levelManager->createLevel(2);

    REQUIRE( barriers->getItems().size() == 40 );
    REQUIRE( eatLocation->isFieldEmpty(4, 24) == false );
    REQUIRE( eatLocation->isFieldEmpty(5, 24) == false );
    REQUIRE( eatLocation->isFieldEmpty(4, 25) == true );
    REQUIRE( eatLocation->isFieldEmpty(5, 25) == true );
}