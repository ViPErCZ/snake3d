cmake_minimum_required(VERSION 3.6)
project(Snake)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES
    include/fString.cpp
    include/fString.h
    CubeSprite.cpp
    CubeSprite.h
    Eat.cpp
    Eat.h
    Engine.cpp
    Engine.h
    GameField.cpp
    GameField.h
    PolygonSprite.cpp
    PolygonSprite.h
        Snake.cpp
    Snake.h
    SnakeContainer.cpp
    SnakeContainer.h
    Sprite.cpp
    Sprite.h
    stdafx.cpp
    stdafx.h
    TextSprite.cpp
    TextSprite.h TTFSprite.cpp TTFSprite.h HeadSprite.cpp HeadSprite.h Wall.cpp Wall.h Radar.cpp Radar.h)

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})
string(STRIP ${SDL2_LIBRARIES} SDL2_LIBRARIES)

add_executable(Snake ${SOURCE_FILES})
target_link_libraries(${PROJECT_NAME} SDL2main SDL2 SDL2_image SDL2_ttf glut GLU GL)