cmake_minimum_required(VERSION 3.22)
project(Snake3)

set(CMAKE_CXX_STANDARD 23)

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})
string(STRIP ${SDL2_LIBRARIES} SDL2_LIBRARIES)

add_executable(Snake3 main.cpp ItemsDto/BaseItem.cpp ItemsDto/BaseItem.h stdafx.h ItemsDto/Cube.cpp ItemsDto/Cube.h App.cpp App.h Manager/ResourceManager.cpp Manager/ResourceManager.h ItemsDto/GameField.cpp ItemsDto/GameField.h Renderer/Opengl/GameFieldRenderer.cpp Renderer/Opengl/GameFieldRenderer.h Renderer/Opengl/BaseRenderer.cpp Renderer/Opengl/BaseRenderer.h Manager/RenderManager.cpp Manager/RenderManager.h ItemsDto/Snake.cpp ItemsDto/Snake.h Renderer/Opengl/SnakeRenderer.cpp Renderer/Opengl/SnakeRenderer.h ItemsDto/Wall.cpp ItemsDto/Wall.h Renderer/Opengl/WallRenderer.cpp Renderer/Opengl/WallRenderer.h Manager/KeyboardManager.cpp Manager/KeyboardManager.h Handler/SnakeMoveHandler.cpp Handler/SnakeMoveHandler.h Handler/BaseHandler.cpp Handler/BaseHandler.h Physic/CollisionDetector.cpp Physic/CollisionDetector.h ItemsDto/BaseContainer.cpp ItemsDto/BaseContainer.h ItemsDto/BaseContainerInterface.cpp ItemsDto/BaseContainerInterface.h Renderer/Opengl/EatRenderer.cpp Renderer/Opengl/EatRenderer.h Renderer/Opengl/TextRenderer.cpp Renderer/Opengl/TextRenderer.h ItemsDto/Text.cpp ItemsDto/Text.h Manager/EatManager.cpp Manager/EatManager.h Handler/EatLocationHandler.cpp Handler/EatLocationHandler.h ItemsDto/Eat.cpp ItemsDto/Eat.h ItemsDto/ObjItem.cpp ItemsDto/ObjItem.h ItemsDto/Radar.cpp ItemsDto/Radar.h Renderer/Opengl/RadarRenderer.cpp Renderer/Opengl/RadarRenderer.h Handler/RadarHandler.cpp Handler/RadarHandler.h Handler/BaseKeydownHandle.cpp Handler/BaseKeydownHandle.h Renderer/Opengl/EatRemoveAnimateRenderer.cpp Renderer/Opengl/EatRemoveAnimateRenderer.h ItemsDto/Barriers.cpp ItemsDto/Barriers.h Renderer/Opengl/BarrierRenderer.cpp Renderer/Opengl/BarrierRenderer.h Manager/ShaderManager.cpp Manager/ShaderManager.h Manager/LevelManager.cpp Manager/LevelManager.h Manager/VboIndexer.cpp Manager/VboIndexer.h ItemsDto/ObjWall.cpp ItemsDto/ObjWall.h Renderer/Opengl/ObjWallRenderer.cpp Renderer/Opengl/ObjWallRenderer.h)
target_link_libraries(${PROJECT_NAME} SDL2main SDL2 SDL2_image SDL2_ttf glut GLU GL GLEW)

file(COPY Assets DESTINATION ${CMAKE_BINARY_DIR})